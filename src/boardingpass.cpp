/*
   SPDX-FileCopyrightText: 2017-2018 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "boardingpass.h"
#include "pass_p.h"

using namespace KPkPass;

BoardingPass::BoardingPass(QObject *parent)
    : Pass(Pass::BoardingPass, parent)
{
}

BoardingPass::~BoardingPass() = default;

BoardingPass::TransitType BoardingPass::transitType() const
{
    const auto t = d->passData().value(QLatin1StringView("transitType")).toString();
    if (t == QLatin1StringView("PKTransitTypeAir")) {
        return Air;
    } else if (t == QLatin1StringView("PKTransitTypeBoat")) {
        return Boat;
    } else if (t == QLatin1StringView("PKTransitTypeBus")) {
        return Bus;
    } else if (t == QLatin1StringView("PKTransitTypeTrain")) {
        return Train;
    }
    return Generic;
}

#include "moc_boardingpass.cpp"
