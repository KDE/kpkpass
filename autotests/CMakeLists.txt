# SPDX-FileCopyrightText: 2018-2021 Volker Krause <vkrause@kde.org>
# SPDX-FileCopyrightText: 2021-2025 Laurent Montel <montel@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

find_package(Qt6Test ${QT_REQUIRED_VERSION} CONFIG REQUIRED)
add_definitions(-DSOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

ecm_add_test(pkpasstest.cpp LINK_LIBRARIES Qt::Test KPim6::PkPass)
ecm_add_test(fieldtest.cpp LINK_LIBRARIES Qt::Test KPim6::PkPass)
