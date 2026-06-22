/*
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "field.h"
#include "pass.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QTest>
#include <QTimeZone>

#include <cmath>

void initLocale()
{
    qputenv("TZ", "UTC");
}

Q_CONSTRUCTOR_FUNCTION(initLocale)

using namespace Qt::Literals;

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
        QCOMPARE(f.value().userType(), QMetaType::QDateTime);
        QCOMPARE(f.value(), QDateTime({2021, 6, 27}, {14, 30}, QTimeZone::fromSecondsAheadOfUtc(7200)));

        obj = QJsonDocument::fromJson(R"({"key":"valid-locations","label":"Ort","value":"Freibad Killesberg\n"})").object();
        f = KPkPass::Field(obj, pass.get());
        QCOMPARE(f.value().userType(), QMetaType::QString);
        QCOMPARE(f.value(), QLatin1StringView("Freibad Killesberg\n"));
        QCOMPARE(f.valueDisplayString(), QLatin1StringView("Freibad Killesberg"));
        QCOMPARE(f.row(), 0);

        obj = QJsonDocument::fromJson(R"({"key":"booking-number","label":"Buchungsnummer","value":1234567894})").object();
        f = KPkPass::Field(obj, pass.get());
        QCOMPARE(f.value().userType(), QMetaType::Double);
        QCOMPARE(f.valueDisplayString(), "1234567894"_L1);
    }

    static void testDateTimeFormat_data()
    {
        QTest::addColumn<QByteArray>("field");
        QTest::addColumn<QString>("output");

        QTest::newRow("implicit-short-date-only")
            << R"({"key":"valid-date","label":"Datum","dateStyle":"PKDateStyleShort","value":"2021-06-27T00:00:00+02:00"})"_ba << u"27/06/2021"_s;
        QTest::newRow("explicit-short-date-only")
            << R"({"key":"valid-date","label":"Datum","dateStyle":"PKDateStyleShort","value":"2021-06-27T12:35:00+02:00","timeStyle":"PKDateStyleNone"})"_ba
            << u"27/06/2021"_s;
        QTest::newRow("implicit-time-only") << R"({"key":"valid-date","label":"Datum","timeStyle":"PKDateStyleShort","value":"2021-06-27T12:34:00+02:00"})"_ba
                                            << u"12:34"_s;
        QTest::newRow("explicit-time-only")
            << R"({"key":"valid-date","label":"Datum","dateStyle":"PKDateStyleNone","timeStyle":"PKDateStyleShort","value":"2021-06-27T12:34:00+02:00"})"_ba
            << u"12:34"_s;
        QTest::newRow("implicit-all") << R"({"key":"valid-date","label":"Datum","value":"2021-06-27T12:34:00+02:00"})"_ba << u"27/06/2021 12:34"_s;
        QTest::newRow("explicit-long-all")
            << R"({"key":"valid-date","label":"Datum","value":"2021-06-27T12:34:00+02:00","dateStyle":"PKDateStyleFull","timeStyle":"PKDateStyleLong"})"_ba
            << u"dimanche 27 juin 2021 12:34:00 UTC+02:00"_s;
    }

    static void testDateTimeFormat()
    {
        QFETCH(QByteArray, field);
        QFETCH(QString, output);

        std::unique_ptr<KPkPass::Pass> pass(KPkPass::Pass::fromFile(QStringLiteral(SOURCE_DIR "/data/boardingpass-v1.pkpass")));
        QVERIFY(pass);
        const auto obj = QJsonDocument::fromJson(field).object();
        const auto f = KPkPass::Field(obj, pass.get());
        QCOMPARE(f.value().userType(), QMetaType::QDateTime);
        QCOMPARE(f.valueDisplayString(), output);
    }

    static void testIsRichText_data()
    {
        QTest::addColumn<QString>("value");
        QTest::addColumn<bool>("output");

        QTest::newRow("empty") << QString() << false;
        QTest::newRow("plain") << u"Frankfurt (Main) Hbf"_s << false;
        QTest::newRow("link") << u"<a href=\"https://maps.apple.com/?q=Frankfurt+%28Main%29+Hbf&ll=50.107149%2C8.663785\">Frankfurt (Main) Hbf</a>"_s << true;
        QTest::newRow("format") << u"bla <b>bla</b> bla"_s << true;
        QTest::newRow("entity") << u"Via: &lt;1080&gt;(L*EF*FD/SDL*WOB*H*GOE*KS)"_s << true;
    }

    static void testIsRichText()
    {
        QFETCH(QString, value);
        QFETCH(bool, output);

        std::unique_ptr<KPkPass::Pass> pass(KPkPass::Pass::fromFile(QStringLiteral(SOURCE_DIR "/data/boardingpass-v1.pkpass")));
        QVERIFY(pass);
        const auto obj = QJsonObject{{"value"_L1, value}};
        const auto f = KPkPass::Field(obj, pass.get());
        QCOMPARE(f.isRichText(), output);
    }
};
}

QTEST_GUILESS_MAIN(KPkPass::FieldTest)

#include "fieldtest.moc"
