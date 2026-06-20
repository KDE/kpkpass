# SPDX-FileCopyrightText: 2026 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

ecm_add_qml_module(kpkpassquickplugin
    GENERATE_PLUGIN_SOURCE
    URI "org.kde.pkpass.ui"
    DEPENDENCIES
        "QtQuick"
        "QtQuick.Controls"
    IMPORTS "org.kde.pkpass"
)

ecm_target_qml_sources(kpkpassquickplugin SOURCES
    AuxiliaryFieldsGrid.qml
    FieldLabel.qml
    FieldValue.qml
)

target_link_libraries(
    kpkpassquickplugin
    PRIVATE
        Qt::Quick
        KPim6PkPass
)

ecm_finalize_qml_module(kpkpassquickplugin DESTINATION ${KDE_INSTALL_QMLDIR})
