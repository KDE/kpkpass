# SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

ecm_add_qml_module(kpkpassqmlplugin
    GENERATE_PLUGIN_SOURCE
    URI "org.kde.pkpass"
    DEPENDENCIES "QtCore" "QtQml"
    SOURCES
        types.cpp
        types.h
)

target_link_libraries(
    kpkpassqmlplugin
    PRIVATE
        Qt::Qml
        KPim6PkPass
)

ecm_finalize_qml_module(kpkpassqmlplugin DESTINATION ${KDE_INSTALL_QMLDIR})
