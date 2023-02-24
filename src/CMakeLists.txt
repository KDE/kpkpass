# SPDX-FileCopyrightText: 2018 Volker Krause <vkrause@kde.org>
# SPDX-FileCopyrightText: 2019-2023 Laurent Montel <montel@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

set(KDE_INSTALL_INCLUDEDIR_PIM ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION})
add_library(KPim${KF_MAJOR_VERSION}PkPass)
add_library(KPim${KF_MAJOR_VERSION}::PkPass ALIAS KPim${KF_MAJOR_VERSION}PkPass)

target_sources(KPim${KF_MAJOR_VERSION}PkPass PRIVATE
    barcode.cpp
    boardingpass.cpp
    field.cpp
    location.cpp
    pass.cpp
    pass.h
    pass_p.h
    location.h
    field.h
    boardingpass.h
    barcode.h
)

ecm_qt_declare_logging_category(KPim${KF_MAJOR_VERSION}PkPass
        HEADER logging.h
        IDENTIFIER KPkPass::Log
        CATEGORY_NAME org.kde.pkpass
        DESCRIPTION "KPkPass" EXPORT KPKPASS)


generate_export_header(KPim${KF_MAJOR_VERSION}PkPass BASE_NAME KPkPass)
set_target_properties(KPim${KF_MAJOR_VERSION}PkPass PROPERTIES
    VERSION ${KPKPASS_VERSION}
    SOVERSION ${KPKPASS_SOVERSION}
    EXPORT_NAME PkPass
)
target_include_directories(KPim${KF_MAJOR_VERSION}PkPass INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_PIM}>")
target_link_libraries(KPim${KF_MAJOR_VERSION}PkPass
    PUBLIC
        Qt::Gui
    PRIVATE
        KF${KF_MAJOR_VERSION}::Archive
)

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KPim${KF_MAJOR_VERSION}PkPass PROPERTIES UNITY_BUILD ON)
endif()

ecm_generate_headers(KPkPass_HEADERS
    HEADER_NAMES
        Barcode
        BoardingPass
        Field
        Location
        Pass
    REQUIRED_HEADERS KPkPass_HEADERS
)

install(TARGETS KPim${KF_MAJOR_VERSION}PkPass EXPORT KPim${KF_MAJOR_VERSION}PkPassTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES
    ${KPkPass_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/kpkpass_export.h
    ${CMAKE_BINARY_DIR}/kpkpass_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_PIM}/KPkPass
)

if (NOT ANDROID AND SharedMimeInfo_VERSION_STRING VERSION_LESS 2.2)
    install(FILES application-vnd-apple-pkpass.xml DESTINATION ${KDE_INSTALL_MIMEDIR})
    update_xdg_mimetypes(${KDE_INSTALL_MIMEDIR})
endif()

if (BUILD_QCH)
    ecm_add_qch(
        KPim${KF_MAJOR_VERSION}PkPass_QCH
        NAME KPim${KF_MAJOR_VERSION}PkPass
        BASE_NAME KPim${KF_MAJOR_VERSION}PkPass
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${KPkPass_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        #IMAGE_DIRS "${CMAKE_SOURCE_DIR}/docs/pics"
        LINK_QCHS
            Qt${QT_MAJOR_VERSION}Core_QCH
            Qt${QT_MAJOR_VERSION}Gui_QCH
            Qt${QT_MAJOR_VERSION}Widgets_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            KPKPASS_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

ecm_qt_install_logging_categories(EXPORT KPKPASS FILE org_kde_kpkpass.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})
