/*
   SPDX-FileCopyrightText: 2018 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPKPASS_PASS_P_H
#define KPKPASS_PASS_P_H

#include <QHash>
#include <QJsonObject>
#include <QString>

#include <memory>

class KZip;
class QIODevice;

namespace KPkPass {
class PassPrivate {
public:
    /** The pass data structure of the pass.json file. */
    QJsonObject passData() const;
    /** Localized message for the given key. */
    QString message(const QString &key) const;

    void parse();
    bool parseMessages(const QString &lang);

    QVector<Field> fields(const QLatin1String &fieldType, const Pass *q) const;

    static Pass *fromData(std::unique_ptr<QIODevice> device, QObject *parent);

    std::unique_ptr<QIODevice> buffer;
    std::unique_ptr<KZip> zip;
    QJsonObject passObj;
    QHash<QString, QString> messages;
    Pass::Type passType;
};
}

#endif // KPKPASS_PASS_H

