/*
   SPDX-FileCopyrightText: 2017-2018 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "kpkpass_export.h"

#include <QMetaType>
#include <QString>

#include <memory>

class QJsonObject;

namespace KPkPass
{
class Pass;
class PassPrivate;
class FieldPrivate;
class FieldTest;

/*! Field element in a KPkPass::Pass.
 * \sa https://developer.apple.com/library/content/documentation/UserExperience/Reference/PassKit_Bundle/Chapters/FieldDictionary.html
 */
class KPKPASS_EXPORT Field
{
    Q_GADGET
    Q_PROPERTY(QString key READ key CONSTANT)
    Q_PROPERTY(QString label READ label CONSTANT)
    Q_PROPERTY(QVariant value READ value CONSTANT)
    Q_PROPERTY(QString valueDisplayString READ valueDisplayString CONSTANT)
    Q_PROPERTY(QString changeMessage READ changeMessage CONSTANT)
    Q_PROPERTY(Qt::Alignment textAlignment READ textAlignment CONSTANT)
    Q_PROPERTY(QString currencyCode READ currencyCode CONSTANT)

public:
    Field();
    Field(const Field &);
    Field(Field &&);
    ~Field();
    Field &operator=(const Field &);

    /*! Field key, unique in the pass but not meant for display. */
    [[nodiscard]] QString key() const;
    /*! Localized label for display describing this field. */
    [[nodiscard]] QString label() const;

    /*! Value of this field.
     *  This can either be a localized string (most common), a date/time value or a number.
     *  Use this for data extraction, prefer valueDisplayString() for displaying data.
     */
    [[nodiscard]] QVariant value() const;
    /*! Value of this field, as a localized string for display.
     *  Use this rather than value() for display.
     */
    [[nodiscard]] QString valueDisplayString() const;

    /*! The localized change message for this value. */
    [[nodiscard]] QString changeMessage() const;

    /*! Text alignment. */
    [[nodiscard]] Qt::Alignment textAlignment() const;

    /*! Currency code. */
    [[nodiscard]] QString currencyCode() const;

private:
    friend class PassPrivate;
    friend class FieldTest;
    explicit Field(const QJsonObject &obj, const Pass *pass);

    std::shared_ptr<FieldPrivate> d;
};

}
