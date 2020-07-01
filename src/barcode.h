/*
    SPDX-FileCopyrightText: 2018 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPKPASS_BARCODE_H
#define KPKPASS_BARCODE_H

#include "kpkpass_export.h"

#include <QMetaType>
#include <QString>

#include <memory>

class QJsonObject;

namespace KPkPass {

class BarcodePrivate;
class Pass;

/** A pass barcode element.
 *  @see https://developer.apple.com/library/content/documentation/UserExperience/Reference/PassKit_Bundle/Chapters/LowerLevel.html
 */
class KPKPASS_EXPORT Barcode
{
    Q_GADGET
    Q_PROPERTY(QString alternativeText READ alternativeText CONSTANT)
    Q_PROPERTY(Format format READ format CONSTANT)
    Q_PROPERTY(QString message READ message CONSTANT)
    Q_PROPERTY(QString messageEncoding READ messageEncoding CONSTANT)

public:
    enum Format {
        Invalid,
        QR,
        PDF417,
        Aztec,
        Code128
    };
    Q_ENUM(Format)

    Barcode();
    ~Barcode();

    /** A human readable version of the barcode data. */
    Q_REQUIRED_RESULT QString alternativeText() const;
    /** The barcode type. */
    Q_REQUIRED_RESULT Format format() const;
    /** The message encoded in the barcode. */
    Q_REQUIRED_RESULT QString message() const;
    /** Encoding used for the message() content. */
    Q_REQUIRED_RESULT QString messageEncoding() const;

private:
    friend class Pass;
    explicit Barcode(const QJsonObject &obj, const Pass *file);
    std::shared_ptr<BarcodePrivate> d;
};

}

Q_DECLARE_METATYPE(KPkPass::Barcode)

#endif // KPKPASS_BARCODE_H
