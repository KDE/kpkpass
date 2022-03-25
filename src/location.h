/*
    SPDX-FileCopyrightText: 2018 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "kpkpass_export.h"

#include <QMetaType>

#include <memory>

class QJsonObject;

namespace KPkPass
{
class LocationPrivate;

/** A pass location element.
 *  @see https://developer.apple.com/library/content/documentation/UserExperience/Reference/PassKit_Bundle/Chapters/LowerLevel.html
 */
class KPKPASS_EXPORT Location
{
    Q_GADGET
    Q_PROPERTY(double altitude READ altitude CONSTANT)
    Q_PROPERTY(double latitude READ latitude CONSTANT)
    Q_PROPERTY(double longitude READ longitude CONSTANT)
    Q_PROPERTY(QString relevantText READ relevantText CONSTANT)
public:
    Location();
    ~Location();

    /** Altitude in meters, NaN if not set. */
    Q_REQUIRED_RESULT double altitude() const;
    /** Latitude in degree. */
    Q_REQUIRED_RESULT double latitude() const;
    /** Longitude in degree. */
    Q_REQUIRED_RESULT double longitude() const;
    /** Text to display when location is reached. */
    Q_REQUIRED_RESULT QString relevantText() const;

private:
    friend class Pass;
    explicit Location(const QJsonObject &obj);
    std::shared_ptr<LocationPrivate> d;
};

}

Q_DECLARE_METATYPE(KPkPass::Location)
