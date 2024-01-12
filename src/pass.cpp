/*
   SPDX-FileCopyrightText: 2017-2018 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "pass.h"
#include "barcode.h"
#include "boardingpass.h"
#include "location.h"
#include "logging.h"
#include "pass_p.h"

#include <KZip>

#include <QBuffer>
#include <QColor>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QRegularExpression>
#include <QStringDecoder>
#include <QUrl>

#include <cctype>

using namespace KPkPass;

static const char *const passTypes[] = {"boardingPass", "coupon", "eventTicket", "generic", "storeCard"};
static const auto passTypesCount = sizeof(passTypes) / sizeof(passTypes[0]);

QJsonObject PassPrivate::passData() const
{
    return passObj.value(QLatin1String(passTypes[passType])).toObject();
}

QString PassPrivate::message(const QString &key) const
{
    const auto it = messages.constFind(key);
    if (it != messages.constEnd()) {
        return it.value();
    }
    return key;
}

void PassPrivate::parse()
{
    // find the first matching message catalog
    const auto langs = QLocale().uiLanguages();
    for (auto lang : langs) {
        auto idx = lang.indexOf(QLatin1Char('-'));
        if (idx > 0) {
            lang = lang.left(idx);
        }
        lang += QLatin1String(".lproj");
        if (parseMessages(lang)) {
            return;
        }
    }

    // fallback to Englis if we didn't find anything better
    parseMessages(QStringLiteral("en.lproj"));
}

static int indexOfUnquoted(const QString &catalog, QLatin1Char c, int start)
{
    for (int i = start; i < catalog.size(); ++i) {
        const QChar catalogChar = catalog.at(i);
        if (catalogChar == c) {
            return i;
        }
        if (catalogChar == QLatin1Char('\\')) {
            ++i;
        }
    }

    return -1;
}

static QString unquote(QStringView str)
{
    QString res;
    res.reserve(str.size());
    for (int i = 0; i < str.size(); ++i) {
        const auto c1 = str.at(i);
        if (c1 == QLatin1Char('\\') && i < str.size() - 1) {
            const auto c2 = str.at(i + 1);
            if (c2 == QLatin1Char('r')) {
                res.push_back(QLatin1Char('\r'));
            } else if (c2 == QLatin1Char('n')) {
                res.push_back(QLatin1Char('\n'));
            } else if (c2 == QLatin1Char('\\')) {
                res.push_back(c2);
            } else {
                res.push_back(c1);
                res.push_back(c2);
            }
            ++i;
        } else {
            res.push_back(c1);
        }
    }
    return res;
}

bool PassPrivate::parseMessages(const QString &lang)
{
    auto entry = zip->directory()->entry(lang);
    if (!entry || !entry->isDirectory()) {
        return false;
    }

    auto dir = static_cast<const KArchiveDirectory *>(entry);
    auto file = dir->file(QStringLiteral("pass.strings"));
    if (!file) {
        return false;
    }

    std::unique_ptr<QIODevice> dev(file->createDevice());
    const auto rawData = dev->readAll();
    if (rawData.size() < 4) {
        return false;
    }
    // this should be UTF-16BE, but that doesn't stop Eurowings from using UTF-8,
    // so do a primitive auto-detection here. UTF-16's first byte would either be the BOM
    // or \0.
    QString catalog;
    if (std::ispunct((unsigned char)rawData.at(0))) {
        catalog = QString::fromUtf8(rawData);
    } else {
        auto codec = QStringDecoder(QStringDecoder::Utf16BE);
        catalog = codec(rawData);
    }

    int idx = 0;
    while (idx < catalog.size()) {
        // key
        const auto keyBegin = indexOfUnquoted(catalog, QLatin1Char('"'), idx) + 1;
        if (keyBegin < 1) {
            break;
        }
        const auto keyEnd = indexOfUnquoted(catalog, QLatin1Char('"'), keyBegin);
        if (keyEnd <= keyBegin) {
            break;
        }

        // value
        const auto valueBegin = indexOfUnquoted(catalog, QLatin1Char('"'), keyEnd + 2) + 1; // there's at least also the '='
        if (valueBegin <= keyEnd) {
            break;
        }
        const auto valueEnd = indexOfUnquoted(catalog, QLatin1Char('"'), valueBegin);
        if (valueEnd < valueBegin) {
            break;
        }

        const auto key = catalog.mid(keyBegin, keyEnd - keyBegin);
        const auto value = unquote(QStringView(catalog).mid(valueBegin, valueEnd - valueBegin));
        messages.insert(key, value);
        idx = valueEnd + 1; // there's at least the linebreak and/or a ';'
    }

    return !messages.isEmpty();
}

QList<Field> PassPrivate::fields(QLatin1String fieldType, const Pass *q) const
{
    const auto a = passData().value(fieldType).toArray();
    QList<Field> f;
    f.reserve(a.size());
    for (const auto &v : a) {
        f.push_back(Field{v.toObject(), q});
    }
    return f;
}

Pass *PassPrivate::fromData(std::unique_ptr<QIODevice> device, QObject *parent)
{
    std::unique_ptr<KZip> zip(new KZip(device.get()));
    if (!zip->open(QIODevice::ReadOnly)) {
        return nullptr;
    }

    // extract pass.json
    auto file = zip->directory()->file(QStringLiteral("pass.json"));
    if (!file) {
        return nullptr;
    }
    std::unique_ptr<QIODevice> dev(file->createDevice());
    QJsonParseError error;
    const auto data = dev->readAll();
    auto passObj = QJsonDocument::fromJson(data, &error).object();
    if (error.error != QJsonParseError::NoError) {
        qCWarning(Log) << "Error parsing pass.json:" << error.errorString() << error.offset;

        // try to fix some known JSON syntax errors
        auto s = QString::fromUtf8(data);
        s.replace(QRegularExpression(QStringLiteral(R"(\}[\s\n]*,[\s\n]*\})")), QStringLiteral("}}"));
        s.replace(QRegularExpression(QStringLiteral(R"(\][\s\n]*,[\s\n]*\})")), QStringLiteral("]}"));
        passObj = QJsonDocument::fromJson(s.toUtf8(), &error).object();
        if (error.error != QJsonParseError::NoError) {
            qCWarning(Log) << "JSON syntax workarounds didn't help either:" << error.errorString() << error.offset;
            return nullptr;
        }
    }
    if (passObj.value(QLatin1String("formatVersion")).toInt() > 1) {
        qCWarning(Log) << "pass.json has unsupported format version!";
        return nullptr;
    }

    // determine pass type
    int passTypeIdx = -1;
    for (unsigned int i = 0; i < passTypesCount; ++i) {
        if (passObj.contains(QLatin1String(passTypes[i]))) {
            passTypeIdx = static_cast<int>(i);
            break;
        }
    }
    if (passTypeIdx < 0) {
        qCWarning(Log) << "pkpass file has no pass data structure!";
        return nullptr;
    }

    Pass *pass = nullptr;
    switch (passTypeIdx) {
    case Pass::BoardingPass:
        pass = new KPkPass::BoardingPass(parent);
        break;
    default:
        pass = new Pass(static_cast<Pass::Type>(passTypeIdx), parent);
        break;
    }

    pass->d->buffer = std::move(device);
    pass->d->zip = std::move(zip);
    pass->d->passObj = passObj;
    pass->d->parse();
    return pass;
}

Pass::Pass(Type passType, QObject *parent)
    : QObject(parent)
    , d(new PassPrivate)
{
    d->passType = passType;
}

Pass::~Pass() = default;

Pass::Type Pass::type() const
{
    return d->passType;
}

QString Pass::description() const
{
    return d->passObj.value(QLatin1String("description")).toString();
}

QString Pass::organizationName() const
{
    return d->passObj.value(QLatin1String("organizationName")).toString();
}

QString Pass::passTypeIdentifier() const
{
    return d->passObj.value(QLatin1String("passTypeIdentifier")).toString();
}

QString Pass::serialNumber() const
{
    return d->passObj.value(QLatin1String("serialNumber")).toString();
}

QDateTime Pass::expirationDate() const
{
    return QDateTime::fromString(d->passObj.value(QLatin1String("expirationDate")).toString(), Qt::ISODate);
}

bool Pass::isVoided() const
{
    return d->passObj.value(QLatin1String("voided")).toString() == QLatin1String("true");
}

QList<Location> Pass::locations() const
{
    QList<Location> locs;
    const auto a = d->passObj.value(QLatin1String("locations")).toArray();
    locs.reserve(a.size());
    for (const auto &loc : a) {
        locs.push_back(Location(loc.toObject()));
    }

    return locs;
}

int Pass::maximumDistance() const
{
    return d->passObj.value(QLatin1String("maxDistance")).toInt(500);
}

QDateTime Pass::relevantDate() const
{
    return QDateTime::fromString(d->passObj.value(QLatin1String("relevantDate")).toString(), Qt::ISODate);
}

static QColor parseColor(const QString &s)
{
    if (s.startsWith(QLatin1String("rgb("), Qt::CaseInsensitive)) {
        const auto l = QStringView(s).mid(4, s.length() - 5).split(QLatin1Char(','));
        if (l.size() != 3)
            return {};
        return QColor(l[0].trimmed().toInt(), l[1].trimmed().toInt(), l[2].trimmed().toInt());
    }
    return QColor(s);
}

QColor Pass::backgroundColor() const
{
    return parseColor(d->passObj.value(QLatin1String("backgroundColor")).toString());
}

QColor Pass::foregroundColor() const
{
    return parseColor(d->passObj.value(QLatin1String("foregroundColor")).toString());
}

QString Pass::groupingIdentifier() const
{
    return d->passObj.value(QLatin1String("groupingIdentifier")).toString();
}

QColor Pass::labelColor() const
{
    const auto c = parseColor(d->passObj.value(QLatin1String("labelColor")).toString());
    if (c.isValid()) {
        return c;
    }
    return foregroundColor();
}

QString Pass::logoText() const
{
    return d->message(d->passObj.value(QLatin1String("logoText")).toString());
}

bool Pass::hasImage(const QString &baseName) const
{
    const auto entries = d->zip->directory()->entries();
    for (const auto &entry : entries) {
        if (entry.startsWith(baseName)
            && (QStringView(entry).mid(baseName.size()).startsWith(QLatin1Char('@')) || QStringView(entry).mid(baseName.size()).startsWith(QLatin1Char('.')))
            && entry.endsWith(QLatin1String(".png"))) {
            return true;
        }
    }
    return false;
}

bool Pass::hasIcon() const
{
    return hasImage(QStringLiteral("icon"));
}

bool Pass::hasLogo() const
{
    return hasImage(QStringLiteral("logo"));
}

bool Pass::hasStrip() const
{
    return hasImage(QStringLiteral("strip"));
}

bool Pass::hasBackground() const
{
    return hasImage(QStringLiteral("background"));
}

bool Pass::hasFooter() const
{
    return hasImage(QStringLiteral("footer"));
}

bool Pass::hasThumbnail() const
{
    return hasImage(QStringLiteral("thumbnail"));
}

QImage Pass::image(const QString &baseName, unsigned int devicePixelRatio) const
{
    const KArchiveFile *file = nullptr;
    QImage img;

    auto dpr = devicePixelRatio;
    for (; dpr > 0; --dpr) {
        const auto it = d->m_images.find(ImageCacheKey{baseName, dpr});
        if (it != d->m_images.end()) {
            img = (*it).second;
            break;
        }
        if (dpr > 1) {
            file = d->zip->directory()->file(baseName + QLatin1Char('@') + QString::number(dpr) + QLatin1String("x.png"));
        } else {
            file = d->zip->directory()->file(baseName + QLatin1String(".png"));
        }
        if (file)
            break;
    }
    if (!img.isNull()) { // cache hit
        return img;
    }

    if (!file) {
        return {};
    }

    std::unique_ptr<QIODevice> dev(file->createDevice());
    img = QImage::fromData(dev->readAll());
    img.setDevicePixelRatio(dpr);
    d->m_images[ImageCacheKey{baseName, dpr}] = img;
    if (dpr != devicePixelRatio) {
        d->m_images[ImageCacheKey{baseName, devicePixelRatio}] = img;
    }
    return img;
}

QImage Pass::icon(unsigned int devicePixelRatio) const
{
    return image(QStringLiteral("icon"), devicePixelRatio);
}

QImage Pass::logo(unsigned int devicePixelRatio) const
{
    return image(QStringLiteral("logo"), devicePixelRatio);
}

QImage Pass::strip(unsigned int devicePixelRatio) const
{
    return image(QStringLiteral("strip"), devicePixelRatio);
}

QImage Pass::background(unsigned int devicePixelRatio) const
{
    return image(QStringLiteral("background"), devicePixelRatio);
}

QImage Pass::footer(unsigned int devicePixelRatio) const
{
    return image(QStringLiteral("footer"), devicePixelRatio);
}

QImage Pass::thumbnail(unsigned int devicePixelRatio) const
{
    return image(QStringLiteral("thumbnail"), devicePixelRatio);
}

QString Pass::authenticationToken() const
{
    return d->passObj.value(QLatin1String("authenticationToken")).toString();
}

QUrl Pass::webServiceUrl() const
{
    return QUrl(d->passObj.value(QLatin1String("webServiceURL")).toString());
}

QUrl Pass::passUpdateUrl() const
{
    QUrl url(webServiceUrl());
    if (!url.isValid()) {
        return {};
    }
    url.setPath(url.path() + QLatin1String("/v1/passes/") + passTypeIdentifier() + QLatin1Char('/') + serialNumber());
    return url;
}

QList<Barcode> Pass::barcodes() const
{
    QList<Barcode> codes;

    // barcodes array
    const auto a = d->passObj.value(QLatin1String("barcodes")).toArray();
    codes.reserve(a.size());
    for (const auto &bc : a)
        codes.push_back(Barcode(bc.toObject(), this));

    // just a single barcode
    if (codes.isEmpty()) {
        const auto bc = d->passObj.value(QLatin1String("barcode")).toObject();
        if (!bc.isEmpty())
            codes.push_back(Barcode(bc, this));
    }

    return codes;
}

static const char *const fieldNames[] = {"auxiliaryFields", "backFields", "headerFields", "primaryFields", "secondaryFields"};
static const auto fieldNameCount = sizeof(fieldNames) / sizeof(fieldNames[0]);

QList<Field> Pass::auxiliaryFields() const
{
    return d->fields(QLatin1String(fieldNames[0]), this);
}

QList<Field> Pass::backFields() const
{
    return d->fields(QLatin1String(fieldNames[1]), this);
}

QList<Field> Pass::headerFields() const
{
    return d->fields(QLatin1String(fieldNames[2]), this);
}

QList<Field> Pass::primaryFields() const
{
    return d->fields(QLatin1String(fieldNames[3]), this);
}

QList<Field> Pass::secondaryFields() const
{
    return d->fields(QLatin1String(fieldNames[4]), this);
}

Field Pass::field(const QString &key) const
{
    for (unsigned int i = 0; i < fieldNameCount; ++i) {
        const auto fs = d->fields(QLatin1String(fieldNames[i]), this);
        for (const auto &f : fs) {
            if (f.key() == key) {
                return f;
            }
        }
    }
    return {};
}

QList<Field> Pass::fields() const
{
    QList<Field> fs;
    for (unsigned int i = 0; i < fieldNameCount; ++i) {
        fs += d->fields(QLatin1String(fieldNames[i]), this);
    }
    return fs;
}

Pass *Pass::fromData(const QByteArray &data, QObject *parent)
{
    std::unique_ptr<QBuffer> buffer(new QBuffer);
    buffer->setData(data);
    buffer->open(QBuffer::ReadOnly);
    return PassPrivate::fromData(std::move(buffer), parent);
}

Pass *Pass::fromFile(const QString &fileName, QObject *parent)
{
    std::unique_ptr<QFile> file(new QFile(fileName));
    if (file->open(QFile::ReadOnly)) {
        return PassPrivate::fromData(std::move(file), parent);
    }
    qCWarning(Log) << "Failed to open" << fileName << ":" << file->errorString();
    return nullptr;
}

QVariantMap Pass::fieldsVariantMap() const
{
    QVariantMap m;
    const auto elems = fields();
    std::for_each(elems.begin(), elems.end(), [&m](const Field &f) {
        m.insert(f.key(), QVariant::fromValue(f));
    });
    return m;
}

QByteArray Pass::rawData() const
{
    const auto prevPos = d->buffer->pos();
    d->buffer->seek(0);
    const auto data = d->buffer->readAll();
    d->buffer->seek(prevPos);
    return data;
}

#include "moc_pass.cpp"
