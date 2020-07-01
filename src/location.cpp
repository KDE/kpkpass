/*
    SPDX-FileCopyrightText: 2018 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "location.h"

#include <QJsonObject>

#include <cmath>

using namespace KPkPass;

namespace KPkPass {
class LocationPrivate {
public:
    QJsonObject obj;
};
}

Location::Location()
    : d(new LocationPrivate)
{
}

Location::Location(const QJsonObject &obj)
    : d(new LocationPrivate)
{
    d->obj = obj;
}

Location::~Location() = default;

double Location::altitude() const
{
    return d->obj.value(QLatin1String("altitude")).toDouble(NAN);
}

double Location::latitude() const
{
    return d->obj.value(QLatin1String("latitude")).toDouble(NAN);
}

double Location::longitude() const
{
    return d->obj.value(QLatin1String("longitude")).toDouble(NAN);
}

QString Location::relevantText() const
{
    return d->obj.value(QLatin1String("relevantText")).toString();
}
