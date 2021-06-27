/*
   SPDX-FileCopyrightText: 2017-2021 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "field.h"
#include "pass.h"
#include "pass_p.h"

#include <QGuiApplication>
#include <QJsonObject>

using namespace KPkPass;

namespace KPkPass
{
class FieldPrivate
{
public:
    const Pass *pass = nullptr;
    QJsonObject obj;
};
}

Field::Field()
    : d(new FieldPrivate)
{
}

Field::Field(const Field &) = default;
Field::Field(Field &&) = default;
Field::~Field() = default;
Field &Field::operator=(const Field &) = default;

Field::Field(const QJsonObject &obj, const Pass *pass)
    : d(new FieldPrivate)
{
    d->pass = pass;
    d->obj = obj;
}

QString Field::key() const
{
    return d->obj.value(QLatin1String("key")).toString();
}

QString Field::label() const
{
    if (d->pass) {
        return d->pass->d->message(d->obj.value(QLatin1String("label")).toString());
    }
    return {};
}

QVariant Field::value() const
{
    if (!d->pass) {
        return {};
    }
    auto v = d->pass->d->message(d->obj.value(QLatin1String("attributedValue")).toString());
    if (v.isEmpty()) {
        v = d->pass->d->message(d->obj.value(QLatin1String("value")).toString());
    }

    const auto dt = QDateTime::fromString(v, Qt::ISODate);
    if (dt.isValid()) {
        return dt;
    }

    // TODO number detection
    return v;
}

QString Field::valueDisplayString() const
{
    const auto v = value();
    if (v.type() == QVariant::DateTime) {
        auto fmt = QLocale::ShortFormat;
        const auto dtStyle = d->obj.value(QLatin1String("dateStyle")).toString();
        if (dtStyle == QLatin1String("PKDateStyleLong") || dtStyle == QLatin1String("PKDateStyleFull")) {
            fmt = QLocale::LongFormat;
        }
        return QLocale().toString(v.toDateTime(), fmt);
    }

    // TODO respect number formatting options
    return value().toString();
}

QString Field::changeMessage() const
{
    if (!d->pass) {
        return {};
    }
    auto msg = d->pass->d->message(d->obj.value(QLatin1String("changeMessage")).toString());
    msg.replace(QLatin1String("%@"), valueDisplayString());
    return msg;
}

Qt::Alignment Field::textAlignment() const
{
    const auto alignStr = d->obj.value(QLatin1String("textAlignment")).toString();
    if (alignStr == QLatin1String("PKTextAlignmentLeft")) {
        return Qt::AlignLeft;
    } else if (alignStr == QLatin1String("PKTextAlignmentCenter")) {
        return Qt::AlignHCenter;
    } else if (alignStr == QLatin1String("PKTextAlignmentRight")) {
        return Qt::AlignRight;
    }
    return QGuiApplication::layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight;
}

#include "moc_field.cpp"
