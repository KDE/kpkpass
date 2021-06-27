/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <field.h>
#include <pass.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QTest>

#include <cmath>

namespace KPkPass
{
class FieldTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
        QLocale::setDefault(QLocale(QStringLiteral("fr_FR")));
    }

    void testBoardingPass()
    {
        std::unique_ptr<KPkPass::Pass> pass(KPkPass::Pass::fromFile(QStringLiteral(SOURCE_DIR "/data/boardingpass-v1.pkpass")));
        QVERIFY(pass);

        auto obj =
            QJsonDocument::fromJson(R"({"key":"valid-date","label":"Datum","dateStyle":"PKDateStyleShort","value":"2021-06-27T14:30:00+02:00"})").object();
        KPkPass::Field f(obj, pass.get());
        QCOMPARE(f.value().type(), QVariant::DateTime);
        QCOMPARE(f.value(), QDateTime({2021, 6, 27}, {14, 30}));
        QCOMPARE(f.valueDisplayString(), QLatin1String("27/06/2021 14:30"));

        obj = QJsonDocument::fromJson(R"({"key":"valid-locations","label":"Ort","value":"Freibad Killesberg\n"})").object();
        f = KPkPass::Field(obj, pass.get());

        QCOMPARE(f.value().type(), QVariant::String);
        QCOMPARE(f.value(), QLatin1String("Freibad Killesberg\n"));
        QCOMPARE(f.valueDisplayString(), QLatin1String("Freibad Killesberg"));
    }
};
}

QTEST_GUILESS_MAIN(KPkPass::FieldTest)

#include "fieldtest.moc"
