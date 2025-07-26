// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QObject>
#include <qqmlintegration.h>

#include <barcode.h>
#include <boardingpass.h>
#include <field.h>

#define FOREIGN_ENUM_GADGET(Class)                                                                                                                             \
    class Class##Derived : public KPkPass::Class                                                                                                               \
    {                                                                                                                                                          \
        Q_GADGET                                                                                                                                               \
    };                                                                                                                                                         \
    namespace Class##DerivedForeign                                                                                                                            \
    {                                                                                                                                                          \
        Q_NAMESPACE                                                                                                                                            \
        QML_NAMED_ELEMENT(Class)                                                                                                                               \
        QML_FOREIGN_NAMESPACE(Class##Derived)                                                                                                                  \
    }

class PassForeign : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Pass)
    QML_UNCREATABLE("")
    QML_FOREIGN(KPkPass::Pass);
};

class BoardingPassForeign : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(BoardingPass)
    QML_UNCREATABLE("")
    QML_FOREIGN(KPkPass::BoardingPass)
};

class FieldForeign
{
    Q_GADGET
    QML_VALUE_TYPE(field)
    QML_FOREIGN(KPkPass::Field)
};

class BarcodeForeign
{
    Q_GADGET
    QML_VALUE_TYPE(barcode)
    QML_FOREIGN(KPkPass::Barcode)
};
FOREIGN_ENUM_GADGET(Barcode)
