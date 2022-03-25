/*
   SPDX-FileCopyrightText: 2017-2018 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "pass.h"

namespace KPkPass
{
/** Boarding pass */
class KPKPASS_EXPORT BoardingPass : public Pass
{
    Q_OBJECT
    Q_PROPERTY(TransitType transitType READ transitType CONSTANT)
public:
    enum TransitType { Air, Boat, Bus, Train, Generic };
    Q_ENUM(TransitType)

    explicit BoardingPass(QObject *parent = nullptr);
    ~BoardingPass() override;

    Q_REQUIRED_RESULT TransitType transitType() const;
};

}
