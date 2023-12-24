/*
   SPDX-FileCopyrightText: 2018 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QHash>
#include <QJsonObject>
#include <QString>

#include <memory>
#include <unordered_map>

class KZip;
class QIODevice;

namespace KPkPass
{
struct ImageCacheKey {
    QString name;
    unsigned int dpr;
    bool operator==(const ImageCacheKey &) const = default;
};
}

template<>
struct std::hash<KPkPass::ImageCacheKey> {
    std::size_t operator()(const KPkPass::ImageCacheKey &key) const noexcept
    {
        return std::hash<QString>{}(key.name) ^ std::hash<unsigned int>{}(key.dpr);
    }
};

namespace KPkPass
{
class PassPrivate
{
public:
    /** The pass data structure of the pass.json file. */
    QJsonObject passData() const;
    /** Localized message for the given key. */
    QString message(const QString &key) const;

    void parse();
    bool parseMessages(const QString &lang);

    QList<Field> fields(QLatin1String fieldType, const Pass *q) const;

    static Pass *fromData(std::unique_ptr<QIODevice> device, QObject *parent);

    std::unique_ptr<QIODevice> buffer;
    std::unique_ptr<KZip> zip;
    QJsonObject passObj;
    QHash<QString, QString> messages;
    Pass::Type passType;
    std::unordered_map<ImageCacheKey, QImage> m_images;
};
}
