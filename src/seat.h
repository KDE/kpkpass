/*
   SPDX-FileCopyrightText: 2026 Volker Krause <vkrause@kde.org>
   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPKPASS_SEAT_H
#define KPKPASS_SEAT_H

#include "kpkpass_export.h"

#include <qobjectdefs.h>

class QJsonObject;

namespace KPkPass
{

class Pass;
class SeatPrivate;

/*!
 * \brief Seat information in semantic tag data in a pkpass.
 *
 * \sa https://developer.apple.com/documentation/walletpasses/semantictagtype/seat-data.dictionary
 * \class KPkPass::Seat
 * \inmodule KPkPass
 * \inheaderfile KPkPass/Seat
 * \since 26.08
 */
class KPKPASS_EXPORT Seat
{
    Q_GADGET
    Q_PROPERTY(QString seatAisle READ seatAisle)
    Q_PROPERTY(QString seatLevel READ seatLevel)
    Q_PROPERTY(QString seatNumber READ seatNumber)
    Q_PROPERTY(QString seatRow READ seatRow)
    Q_PROPERTY(QString seatSection READ seatSection)

public:
    explicit Seat();
    Seat(const Seat &), Seat(Seat &&) noexcept;
    ~Seat();
    Seat &operator=(const Seat &);
    Seat &operator=(Seat &&) noexcept;

    [[nodiscard]] QString seatAisle() const;
    [[nodiscard]] QString seatLevel() const;
    [[nodiscard]] QString seatNumber() const;
    [[nodiscard]] QString seatRow() const;
    [[nodiscard]] QString seatSection() const;

private:
    friend class Pass;
    explicit Seat(const QJsonObject &obj, const Pass *pass);

    std::shared_ptr<SeatPrivate> d;
};

}

#endif
