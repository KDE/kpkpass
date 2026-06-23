/*
    SPDX-FileCopyrightText: 2018 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "barcode.h"
#include "boardingpass.h"
#include "location.h"

#include <QJsonObject>
#include <QLocale>
#include <QTest>
#include <QTimeZone>

#include <cmath>

using namespace Qt::Literals;

class PkPassTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
        QLocale::setDefault(QLocale(QStringLiteral("de_DE")));
    }

    void testBoardingPass()
    {
        std::unique_ptr<KPkPass::Pass> pass(KPkPass::Pass::fromFile(QStringLiteral(SOURCE_DIR "/data/boardingpass-v1.pkpass")));
        QVERIFY(pass);

        QCOMPARE(pass->type(), KPkPass::Pass::BoardingPass);
        QCOMPARE(pass->serialNumber(), QLatin1StringView("1234"));
        QCOMPARE(pass->description(), "KDE Bordkarte"_L1);
        QCOMPARE(pass->lookupMessage(u"description"_s), "KDE Bordkarte"_L1);
        QCOMPARE(pass->organizationName(), QLatin1StringView("KDE"));

        QCOMPARE(pass->logoText(), QLatin1StringView("Boarding Pass"));
        QVERIFY(pass->hasBackgroundColor());
        QCOMPARE(pass->backgroundColor(), QColor(61, 174, 233));
        QVERIFY(pass->hasForegroundColor());
        QVERIFY(pass->hasLabelColor());
        QCOMPARE(pass->relevantDate(), QDateTime(QDate(2017, 9, 17), QTime(0, 4, 0), QTimeZone::UTC));
        QCOMPARE(pass->expirationDate(), QDateTime(QDate(2017, 9, 18), QTime(0, 0, 36), QTimeZone::UTC));
        QCOMPARE(pass->isVoided(), false);
        QCOMPARE(pass->groupingIdentifier(), QLatin1StringView(""));

        QCOMPARE(pass->fields().size(), 12);
        auto headers = pass->headerFields();
        QCOMPARE(headers.size(), 2);
        auto field = headers.at(0);
        QCOMPARE(field.label(), QLatin1StringView("Sitzplatz"));
        QCOMPARE(field.value().toString(), QLatin1StringView("10E"));
        QCOMPARE(field.valueDisplayString(), QLatin1StringView("10E"));
        QCOMPARE(field.key(), QLatin1StringView("seat"));
        QCOMPARE(field.changeMessage(), QStringLiteral("Sitzplatznummer geändert in 10E"));

        field = pass->field(QStringLiteral("boarding"));
        QCOMPARE(field.key(), QLatin1StringView("boarding"));
        QCOMPARE(field.value().toString(), QLatin1StringView("20:25"));
        QCOMPARE(field.textAlignment(), Qt::AlignLeft);

        QCOMPARE(pass->auxiliaryFieldsRowCount(), 1);
        QCOMPARE(pass->auxiliaryFields().size(), 4);
        QCOMPARE(pass->auxiliaryFieldsInRow(0).size(), 4);
        QCOMPARE(pass->auxiliaryFieldsInRow(1).size(), 0);

        auto boardingPass = dynamic_cast<KPkPass::BoardingPass *>(pass.get());
        QVERIFY(boardingPass);
        QCOMPARE(boardingPass->transitType(), KPkPass::BoardingPass::Air);

        QVERIFY(pass->hasBarcode());
        auto barcodes = pass->barcodes();
        QCOMPARE(barcodes.size(), 1);
        auto bc = barcodes.at(0);
        QCOMPARE(bc.format(), KPkPass::Barcode::QR);
        QVERIFY(!bc.message().isEmpty());
        QVERIFY(bc.alternativeText().isEmpty());
        QCOMPARE(bc.messageEncoding(), QLatin1StringView("iso-8859-1"));

        const auto locs = pass->locations();
        QCOMPARE(locs.size(), 1);
        const auto loc = locs.at(0);
        QVERIFY(std::isnan(loc.altitude()));
        QCOMPARE((int)loc.latitude(), 47);
        QCOMPARE((int)loc.longitude(), 8);
        QCOMPARE(loc.relevantText(), QLatin1StringView("LX962 Boarding 20:25"));
        QCOMPARE(pass->maximumDistance(), 500);

        QCOMPARE(pass->hasIcon(), false);
        QCOMPARE(pass->hasLogo(), true);
        auto img = pass->logo(3);
        QVERIFY(!img.isNull());
        img = pass->logo(3);
        QVERIFY(!img.isNull());
        img = pass->logo(1);
        QVERIFY(!img.isNull());

        img = pass->image(QStringLiteral("I don't exist"));
        QVERIFY(img.isNull());

        QVERIFY(pass->preferredStyleSchemes().isEmpty());

        auto sourceFile = QFile(QStringLiteral(SOURCE_DIR "/data/boardingpass-v1.pkpass"));
        QVERIFY(sourceFile.open(QFile::ReadOnly));
        QCOMPARE(pass->rawData(), sourceFile.readAll());
    }

    static void testSemanticTags()
    {
        std::unique_ptr<KPkPass::Pass> pass(KPkPass::Pass::fromFile(u"" SOURCE_DIR "/data/apple-store-UA-sample-unsigned-scrubbed.pkpass"_s));
        QVERIFY(pass);
        const auto semanticTags = pass->semanticTags();
        QVERIFY(!semanticTags.isEmpty());
        QCOMPARE(semanticTags.value("departureAirportName"_L1).toString(), "O'Hare International Airport"_L1);

        QCOMPARE(pass->rawValue("changeSeatURL"_L1).toString(), "https://www.united.com/en/us/fly/travel/trip-planning/seat-options-and-upgrades.html"_L1);

        QVERIFY(pass->hasBarcode());
        QCOMPARE(pass->barcodes().size(), 1);
        QCOMPARE(pass->barcodes().front().format(), KPkPass::Barcode::Aztec);

        const auto styles = pass->preferredStyleSchemes();
        QCOMPARE(styles.size(), 2);
        QCOMPARE(styles.front(), "semanticBoardingPass"_L1);
    }
};

QTEST_GUILESS_MAIN(PkPassTest)

#include "pkpasstest.moc"
