/*
   SPDX-FileCopyrightText: ⓒ 2025 Volker Krause <vkrause@kde.org>
   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPKPASS_PASSES_H
#define KPKPASS_PASSES_H

#include "kpkpass_export.h"

#include <QStringList>

#include <memory>

class QByteArray;
class QString;

namespace KPkPass
{

class PassesPrivate;

/*! Representes a .pkpasses multi-pass bundle file.
 *  \sa https://developer.apple.com/documentation/walletpasses/distributing-and-updating-a-pass#Create-a-bundle-of-passes
 *  \since 25.12
 */
class KPKPASS_EXPORT Passes
{
public:
    Passes(Passes &&) noexcept;
    ~Passes();
    Passes &operator=(Passes &&) noexcept;

    /*! Lists the names of all contained passes. */
    [[nodiscard]] QStringList entries() const;

    /*! Returns the raw data of a pass with \a name. */
    [[nodiscard]] QByteArray passData(const QString &name) const;

    /*! Create a new passes bundle from \a data. */
    [[nodiscard]] static Passes *fromData(const QByteArray &data);

private:
    explicit Passes(std::unique_ptr<PassesPrivate> &&dd);
    std::unique_ptr<PassesPrivate> d;
};

}

#endif
