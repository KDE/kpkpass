/*
   Copyright (c) 2017-2018 Volker Krause <vkrause@kde.org>

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU Library General Public License as published by
   the Free Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   This library is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
   License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/

#ifndef KPKPASS_PASS_H
#define KPKPASS_PASS_H

#include "kpkpass_export.h"
#include "field.h"

#include <QObject>
#include <QVector>

#include <memory>

class QByteArray;
class QColor;
class QDateTime;
class QString;
class QUrl;
class QVariant;

namespace KPkPass {

class Barcode;
class Location;
class PassPrivate;

/** Base class for a pkpass file.
 *  @see https://developer.apple.com/library/archive/documentation/UserExperience/Conceptual/PassKit_PG/index.html
 *  @see https://developer.apple.com/library/content/documentation/UserExperience/Reference/PassKit_Bundle/Chapters/TopLevel.html
 */
class KPKPASS_EXPORT Pass : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Type type READ type CONSTANT)

    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QString organizationName READ organizationName CONSTANT)
    Q_PROPERTY(QString passTypeIdentifier READ passTypeIdentifier CONSTANT)
    Q_PROPERTY(QString serialNumber READ serialNumber CONSTANT)

    Q_PROPERTY(QDateTime expirationDate READ expirationDate CONSTANT)
    Q_PROPERTY(bool isVoided READ isVoided CONSTANT)

    Q_PROPERTY(QDateTime relevantDate READ relevantDate CONSTANT)

    Q_PROPERTY(QColor backgroundColor READ backgroundColor CONSTANT)
    Q_PROPERTY(QColor foregroundColor READ foregroundColor CONSTANT)
    Q_PROPERTY(QString groupingIdentifier READ groupingIdentifier CONSTANT)
    Q_PROPERTY(QColor labelColor READ labelColor CONSTANT)
    Q_PROPERTY(QString logoText READ logoText CONSTANT)

    // needs to be QVariantList just for QML (Grantlee would also work with QVector<Field>
    Q_PROPERTY(QVariantList barcodes READ barcodesVariant CONSTANT)
    Q_PROPERTY(QVariantList auxiliaryFields READ auxiliaryFieldsVariant CONSTANT)
    Q_PROPERTY(QVariantList backFields READ backFieldsVariant CONSTANT)
    Q_PROPERTY(QVariantList headerFields READ headerFieldsVariant CONSTANT)
    Q_PROPERTY(QVariantList primaryFields READ primaryFieldsVariant CONSTANT)
    Q_PROPERTY(QVariantList secondaryFields READ secondaryFieldsVariant CONSTANT)
    Q_PROPERTY(QVariantList locations READ locationsVariant CONSTANT)
    Q_PROPERTY(QVariantMap field READ fieldsVariantMap CONSTANT)

public:
    virtual ~Pass();

    /** Type of the pass. */
    enum Type {
        BoardingPass,
        Coupon,
        EventTicket,
        Generic,
        StoreCard
    };
    Q_ENUM(Type)
    Q_REQUIRED_RESULT Type type() const;

    // standard keys
    Q_REQUIRED_RESULT QString description() const;
    Q_REQUIRED_RESULT QString organizationName() const;
    Q_REQUIRED_RESULT QString passTypeIdentifier() const;
    Q_REQUIRED_RESULT QString serialNumber() const;

    // expiration keys
    Q_REQUIRED_RESULT QDateTime expirationDate() const;
    Q_REQUIRED_RESULT bool isVoided() const;

    // relevance keys
    /** Locations associated with this pass. */
    Q_REQUIRED_RESULT QVector<Location> locations() const;
    /** Distance in meters to any of the pass locations before this pass becomes relevant. */
    Q_REQUIRED_RESULT int maximumDistance() const;
    Q_REQUIRED_RESULT QDateTime relevantDate() const;

    // visual appearance keys
    /** Returns all barcodes defined in the pass. */
    Q_REQUIRED_RESULT QVector<Barcode> barcodes() const;
    Q_REQUIRED_RESULT QColor backgroundColor() const;
    Q_REQUIRED_RESULT QColor foregroundColor() const;
    Q_REQUIRED_RESULT QString groupingIdentifier() const;
    Q_REQUIRED_RESULT QColor labelColor() const;
    Q_REQUIRED_RESULT QString logoText() const;

    /** Returns an image asset of this pass.
     *  @param baseName The name of the asset, without the file name extension.
     *  @param devicePixelRatio The device pixel ration, for loading highdpi assets.
     */
    QImage image(const QString &baseName, unsigned int devicePixelRatio = 1) const;
    /** Returns the pass icon. */
    QImage icon(unsigned int devicePixelRatio = 1) const;
    /** Returns the pass logo. */
    QImage logo(unsigned int devicePixelRatio = 1) const;
    /** Returns the strip image if present. */
    QImage strip(unsigned int devicePixelRatio = 1) const;
    /** Returns the background image if present. */
    QImage background(unsigned int devicePixelRatio = 1) const;
    /** Returns the footer image if present. */
    QImage footer(unsigned int devicePixelRatio = 1) const;
    /** Returns the thumbnail image if present. */
    QImage thumbnail(unsigned int devicePixelRatio = 1) const;

    // web service keys
    Q_REQUIRED_RESULT QString authenticationToken() const;
    Q_REQUIRED_RESULT QUrl webServiceUrl() const;
    /** Pass update URL.
     * @see https://developer.apple.com/library/content/documentation/PassKit/Reference/PassKit_WebService/WebService.html
     */
    Q_REQUIRED_RESULT QUrl passUpdateUrl() const;

    QVector<Field> auxiliaryFields() const;
    QVector<Field> backFields() const;
    QVector<Field> headerFields() const;
    QVector<Field> primaryFields() const;
    QVector<Field> secondaryFields() const;

    /** Returns the field with key @p key. */
    Field field(const QString &key) const;
    /** Returns all fields found in this pass. */
    QVector<Field> fields() const;

    /** Create a appropriate sub-class based on the pkpass file type. */
    static Pass *fromData(const QByteArray &data, QObject *parent = nullptr);
    /** Create a appropriate sub-class based on the pkpass file type. */
    static Pass *fromFile(const QString &fileName, QObject *parent = nullptr);

protected:
    ///@cond internal
    friend class Barcode;
    friend class Field;
    friend class PassPrivate;
    explicit Pass (Type passType, QObject *parent = nullptr);
    std::unique_ptr<PassPrivate> d;
    ///@endcond

private:
    QVariantList auxiliaryFieldsVariant() const;
    QVariantList backFieldsVariant() const;
    QVariantList headerFieldsVariant() const;
    QVariantList primaryFieldsVariant() const;
    QVariantList secondaryFieldsVariant() const;
    QVariantList barcodesVariant() const;
    QVariantList locationsVariant() const;
    QVariantMap fieldsVariantMap() const;
};

}

#endif // KPKPASS_PASS_H
