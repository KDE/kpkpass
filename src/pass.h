/*
   SPDX-FileCopyrightText: 2017-2018 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "field.h"
#include "kpkpass_export.h"

#include <QList>
#include <QObject>

#include <memory>

class QByteArray;
class QColor;
class QDateTime;
class QString;
class QUrl;
class QVariant;

namespace KPkPass
{
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

    Q_PROPERTY(bool hasIcon READ hasIcon CONSTANT)
    Q_PROPERTY(bool hasLogo READ hasLogo CONSTANT)
    Q_PROPERTY(bool hasStrip READ hasStrip CONSTANT)
    Q_PROPERTY(bool hasBackground READ hasBackground CONSTANT)
    Q_PROPERTY(bool hasFooter READ hasFooter CONSTANT)
    Q_PROPERTY(bool hasThumbnail READ hasThumbnail CONSTANT)

    // needs to be QVariantList just for QML (Grantlee would also work with QList<Field>
    Q_PROPERTY(QList<KPkPass::Barcode> barcodes READ barcodes CONSTANT)
    Q_PROPERTY(QList<KPkPass::Field> auxiliaryFields READ auxiliaryFields CONSTANT)
    Q_PROPERTY(QList<KPkPass::Field> backFields READ backFields CONSTANT)
    Q_PROPERTY(QList<KPkPass::Field> headerFields READ headerFields CONSTANT)
    Q_PROPERTY(QList<KPkPass::Field> primaryFields READ primaryFields CONSTANT)
    Q_PROPERTY(QList<KPkPass::Field> secondaryFields READ secondaryFields CONSTANT)
    Q_PROPERTY(QList<KPkPass::Location> locations READ locations CONSTANT)
    Q_PROPERTY(QVariantMap field READ fieldsVariantMap CONSTANT)

public:
    ~Pass() override;

    /** Type of the pass. */
    enum Type {
        BoardingPass,
        Coupon,
        EventTicket,
        Generic,
        StoreCard
    };
    Q_ENUM(Type)
    [[nodiscard]] Type type() const;

    // standard keys
    [[nodiscard]] QString description() const;
    [[nodiscard]] QString organizationName() const;
    [[nodiscard]] QString passTypeIdentifier() const;
    [[nodiscard]] QString serialNumber() const;

    // expiration keys
    [[nodiscard]] QDateTime expirationDate() const;
    [[nodiscard]] bool isVoided() const;

    // relevance keys
    /** Locations associated with this pass. */
    [[nodiscard]] QList<Location> locations() const;
    /** Distance in meters to any of the pass locations before this pass becomes relevant. */
    [[nodiscard]] int maximumDistance() const;
    [[nodiscard]] QDateTime relevantDate() const;

    // visual appearance keys
    /** Returns all barcodes defined in the pass. */
    [[nodiscard]] QList<Barcode> barcodes() const;
    [[nodiscard]] QColor backgroundColor() const;
    [[nodiscard]] QColor foregroundColor() const;
    [[nodiscard]] QString groupingIdentifier() const;
    [[nodiscard]] QColor labelColor() const;
    [[nodiscard]] QString logoText() const;

    /** Returns @c true if an image asset with the given base name exists.
     *  @param baseName The name of the asset, without the file type and high dpi extensions.
     *  @since 5.20.41
     */
    bool hasImage(const QString &baseName) const;
    bool hasIcon() const;
    bool hasLogo() const;
    bool hasStrip() const;
    bool hasBackground() const;
    bool hasFooter() const;
    bool hasThumbnail() const;

    /** Returns an image asset of this pass.
     *  @param baseName The name of the asset, without the file name extension.
     *  @param devicePixelRatio The device pixel ration, for loading highdpi assets.
     */
    [[nodiscard]] QImage image(const QString &baseName, unsigned int devicePixelRatio = 1) const;
    /** Returns the pass icon. */
    Q_INVOKABLE [[nodiscard]] QImage icon(unsigned int devicePixelRatio = 1) const;
    /** Returns the pass logo. */
    Q_INVOKABLE [[nodiscard]] QImage logo(unsigned int devicePixelRatio = 1) const;
    /** Returns the strip image if present. */
    Q_INVOKABLE [[nodiscard]] QImage strip(unsigned int devicePixelRatio = 1) const;
    /** Returns the background image if present. */
    Q_INVOKABLE [[nodiscard]] QImage background(unsigned int devicePixelRatio = 1) const;
    /** Returns the footer image if present. */
    Q_INVOKABLE [[nodiscard]] QImage footer(unsigned int devicePixelRatio = 1) const;
    /** Returns the thumbnail image if present. */
    Q_INVOKABLE [[nodiscard]] QImage thumbnail(unsigned int devicePixelRatio = 1) const;

    // web service keys
    [[nodiscard]] QString authenticationToken() const;
    [[nodiscard]] QUrl webServiceUrl() const;
    /** Pass update URL.
     * @see https://developer.apple.com/library/content/documentation/PassKit/Reference/PassKit_WebService/WebService.html
     */
    [[nodiscard]] QUrl passUpdateUrl() const;

    QList<Field> auxiliaryFields() const;
    QList<Field> backFields() const;
    QList<Field> headerFields() const;
    QList<Field> primaryFields() const;
    QList<Field> secondaryFields() const;

    /** Returns the field with key @p key. */
    Field field(const QString &key) const;
    /** Returns all fields found in this pass. */
    QList<Field> fields() const;

    /** Create a appropriate sub-class based on the pkpass file type. */
    static Pass *fromData(const QByteArray &data, QObject *parent = nullptr);
    /** Create a appropriate sub-class based on the pkpass file type. */
    static Pass *fromFile(const QString &fileName, QObject *parent = nullptr);

    /** The raw data of this pass.
     *  That is the binary representation of the ZIP archive which contains
     *  all the pass data.
     *  @since 5.20.41
     */
    QByteArray rawData() const;

protected:
    ///@cond internal
    friend class Barcode;
    friend class Field;
    friend class PassPrivate;
    explicit Pass(Type passType, QObject *parent = nullptr);
    std::unique_ptr<PassPrivate> d;
    ///@endcond

private:
    QVariantMap fieldsVariantMap() const;
};

}
