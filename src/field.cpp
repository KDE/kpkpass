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
using namespace Qt::Literals;

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
    return d->obj.value(QLatin1StringView("key")).toString();
}

QString Field::label() const
{
    if (d->pass) {
        return d->pass->d->message(d->obj.value(QLatin1StringView("label")).toString());
    }
    return {};
}

QVariant Field::value() const
{
    if (!d->pass) {
        return {};
    }
    auto v = d->obj.value("attributedValue"_L1);
    if (v.isUndefined()) {
        v = d->obj.value("value"_L1);
    }

    if (v.isString()) {
        const auto s = d->pass->d->message(v.toString());
        const auto dt = QDateTime::fromString(s, Qt::ISODate);
        if (dt.isValid()) {
            return dt;
        }
        return s;
    }

    if (v.isDouble()) {
        return v.toDouble();
    }

    return {};
}

QString Field::valueDisplayString() const
{
    const auto v = value();
    // see
    // https://developer.apple.com/library/archive/documentation/UserExperience/Reference/PassKit_Bundle/Chapters/FieldDictionary.html#//apple_ref/doc/uid/TP40012026-CH4-SW6
    // however, real-world data doesn't strictly follow that, so we have to guess a bit here...
    if (v.typeId() == QMetaType::QDateTime) {
        const auto dt = v.toDateTime();
        auto fmt = QLocale::ShortFormat;
        const auto dtStyle = d->obj.value(QLatin1StringView("dateStyle")).toString();
        if (dtStyle == QLatin1StringView("PKDateStyleLong") || dtStyle == QLatin1StringView("PKDateStyleFull")) {
            fmt = QLocale::LongFormat;
        }
        const auto timeStyle = d->obj.value(QLatin1StringView("timeStyle")).toString();
        if (timeStyle == QLatin1StringView("PKDateStyleNone") || (timeStyle.isEmpty() && !dtStyle.isEmpty() && dt.time() == QTime(0, 0))) {
            return QLocale().toString(dt.date(), fmt);
        }

        return QLocale().toString(dt, fmt);
    }
    if (v.typeId() == QMetaType::Double) {
        if (const auto currency = currencyCode(); !currency.isEmpty()) {
            return QLocale().toCurrencyString(v.toDouble(), currency);
        }

        // TODO respect number formatting options
        return QString::number(v.toDouble());
    }

    return v.toString().trimmed();
}

QString Field::changeMessage() const
{
    if (!d->pass) {
        return {};
    }
    auto msg = d->pass->d->message(d->obj.value(QLatin1StringView("changeMessage")).toString());
    msg.replace(QLatin1StringView("%@"), valueDisplayString());
    return msg;
}

Qt::Alignment Field::textAlignment() const
{
    const auto alignStr = d->obj.value(QLatin1StringView("textAlignment")).toString();
    if (alignStr == QLatin1StringView("PKTextAlignmentLeft")) {
        return Qt::AlignLeft;
    } else if (alignStr == QLatin1StringView("PKTextAlignmentCenter")) {
        return Qt::AlignHCenter;
    } else if (alignStr == QLatin1StringView("PKTextAlignmentRight")) {
        return Qt::AlignRight;
    }
    return QGuiApplication::layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight;
}

QString Field::currencyCode() const
{
    return d->obj.value("currencyCode"_L1).toString();
}

#include "moc_field.cpp"
