/*
   SPDX-FileCopyrightText: ⓒ 2025 Volker Krause <vkrause@kde.org>
   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "passes.h"

#include <KZip>

#include <QBuffer>

namespace KPkPass
{
class PassesPrivate
{
public:
    std::unique_ptr<QIODevice> m_ioDevice;
    std::unique_ptr<KZip> m_zip;
};
}

using namespace KPkPass;

Passes::Passes(std::unique_ptr<PassesPrivate> &&dd)
    : d(std::move(dd))
{
}

Passes::Passes(Passes &&) noexcept = default;
Passes::~Passes() = default;

Passes &Passes::operator=(Passes &&) noexcept = default;

QStringList Passes::entries() const
{
    return d->m_zip->directory()->entries();
}

QByteArray Passes::passData(const QString &name) const
{
    const auto file = d->m_zip->directory()->file(name);
    if (!file) {
        return {};
    }

    std::unique_ptr<QIODevice> device(file->createDevice());
    if (!device) {
        return {};
    }
    return device->readAll();
}

Passes *Passes::fromData(const QByteArray &data)
{
    auto buffer = std::make_unique<QBuffer>();
    buffer->setData(data);
    buffer->open(QBuffer::ReadOnly);

    auto d = std::make_unique<PassesPrivate>();
    d->m_ioDevice = std::move(buffer);
    d->m_zip = std::make_unique<KZip>(d->m_ioDevice.get());
    if (!d->m_zip->open(QIODevice::ReadOnly)) {
        return nullptr;
    }

    return new Passes(std::move(d));
}
