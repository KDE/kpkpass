// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QObject>
#include <qqmlintegration.h>

#include <barcode.h>
#include <boardingpass.h>
#include <field.h>

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
