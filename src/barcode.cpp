/*
    SPDX-FileCopyrightText: 2018 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "barcode.h"
#include "pass.h"
#include "pass_p.h"

#include <QJsonObject>

using namespace Qt::Literals;
using namespace KPkPass;

namespace KPkPass
{
class BarcodePrivate
{
public:
    const Pass *pass = nullptr;
    QJsonObject obj;
};
}

Barcode::Barcode()
    : d(new BarcodePrivate)
{
}

Barcode::Barcode(const QJsonObject &obj, const Pass *pass)
    : d(new BarcodePrivate)
{
    d->pass = pass;
    d->obj = obj;
}

Barcode::~Barcode() = default;

QString Barcode::alternativeText() const
{
    if (d->pass) {
        return d->pass->d->message(d->obj.value(QLatin1StringView("altText")).toString());
    }
    return {};
}

struct {
    const char *name;
    KPkPass::Barcode::Format format;
} static constexpr const barcode_formats[] = {
    {"PKBarcodeFormatQR", Barcode::QR},
    {"PKBarcodeFormatPDF417", Barcode::PDF417},
    {"PKBarcodeFormatAztec", Barcode::Aztec},
    {"PKBarcodeFormatCode128", Barcode::Code128},
    {"PKBarcodeFormatCode39", Barcode::Code39},
    {"PKBarcodeFormatCodabar", Barcode::Codabar},
    {"PKBarcodeFormatEAN13", Barcode::EAN13},
    {"PKBarcodeFormatI2of5", Barcode::I2of5},
};

Barcode::Format KPkPass::Barcode::format() const
{
    const auto format = d->obj.value("format"_L1);
    const auto it = std::ranges::find_if(barcode_formats, [format](const auto &f) {
        return QLatin1StringView(f.name) == format;
    });
    return it != std::end(barcode_formats) ? (*it).format : Invalid;
}

QString Barcode::message() const
{
    return d->obj.value(QLatin1StringView("message")).toString();
}

QString Barcode::messageEncoding() const
{
    return d->obj.value(QLatin1StringView("messageEncoding")).toString();
}

#include "moc_barcode.cpp"
