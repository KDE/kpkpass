/*
    SPDX-FileCopyrightText: 2018 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "barcode.h"
#include "boardingpass.h"
#include "location.h"

#include <QLocale>
#include <QTest>
#include <QTimeZone>

#include <cmath>

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
        QCOMPARE(pass->description(), QLatin1StringView("description"));
        QCOMPARE(pass->organizationName(), QLatin1StringView("KDE"));

        QCOMPARE(pass->logoText(), QLatin1StringView("Boarding Pass"));
        QCOMPARE(pass->backgroundColor(), QColor(61, 174, 233));
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

        auto boardingPass = dynamic_cast<KPkPass::BoardingPass *>(pass.get());
        QVERIFY(boardingPass);
        QCOMPARE(boardingPass->transitType(), KPkPass::BoardingPass::Air);

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

        auto sourceFile = QFile(QStringLiteral(SOURCE_DIR "/data/boardingpass-v1.pkpass"));
        QVERIFY(sourceFile.open(QFile::ReadOnly));
        QCOMPARE(pass->rawData(), sourceFile.readAll());
    }
};

QTEST_GUILESS_MAIN(PkPassTest)

#include "pkpasstest.moc"
