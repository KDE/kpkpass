/*
    SPDX-FileCopyrightText: 2018 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "barcode.h"
#include "pass.h"
#include "pass_p.h"

#include <QJsonObject>

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

Barcode::Format KPkPass::Barcode::format() const
{
    const auto format = d->obj.value(QLatin1StringView("format")).toString();
    if (format == QLatin1StringView("PKBarcodeFormatQR")) {
        return QR;
    } else if (format == QLatin1StringView("PKBarcodeFormatPDF417")) {
        return PDF417;
    } else if (format == QLatin1StringView("PKBarcodeFormatAztec")) {
        return Aztec;
    } else if (format == QLatin1StringView("PKBarcodeFormatCode128")) {
        return Code128;
    }
    return Invalid;
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
