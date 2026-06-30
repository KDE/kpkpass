/*
   SPDX-FileCopyrightText: 2026 Volker Krause <vkrause@kde.org>
   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "seat.h"
#include "pass.h"
#include "pass_p.h"

#include <QJsonObject>

using namespace Qt::Literals;
using namespace KPkPass;

namespace KPkPass
{
class SeatPrivate
{
public:
    const Pass *pass = nullptr;
    QJsonObject obj;
};
}

Seat::Seat()
    : d(new SeatPrivate)
{
}

Seat::Seat(const Seat &) = default;
Seat::Seat(Seat &&) noexcept = default;
Seat::~Seat() = default;
Seat &Seat::operator=(const Seat &) = default;
Seat &Seat::operator=(Seat &&) noexcept = default;

Seat::Seat(const QJsonObject &obj, const Pass *pass)
    : d(new SeatPrivate)
{
    d->pass = pass;
    d->obj = obj;
}

bool Seat::hasSeatAisle() const
{
    return !seatAisle().isEmpty();
}

bool Seat::hasSeatLevel() const
{
    return !seatLevel().isEmpty();
}

bool Seat::hasSeatNumber() const
{
    return !seatNumber().isEmpty();
}

bool Seat::hasSeatRow() const
{
    return !seatRow().isEmpty();
}

bool Seat::hasSeatSection() const
{
    return !seatSection().isEmpty();
}

QString Seat::seatAisle() const
{
    return d->pass->d->message(d->obj.value("seatAisle"_L1).toString());
}

QString Seat::seatLevel() const
{
    return d->pass->d->message(d->obj.value("seatLevel"_L1).toString());
}

QString Seat::seatNumber() const
{
    return d->pass->d->message(d->obj.value("seatNumber"_L1).toString());
}

QString Seat::seatRow() const
{
    return d->pass->d->message(d->obj.value("seatRow"_L1).toString());
}

QString Seat::seatSection() const
{
    return d->pass->d->message(d->obj.value("seatSection"_L1).toString());
}

QString Seat::asAirplaneSeat() const
{
    return seatRow() + seatNumber();
}

#include "moc_seat.cpp"
