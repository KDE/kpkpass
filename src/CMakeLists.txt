# SPDX-FileCopyrightText: 2018 Volker Krause <vkrause@kde.org>
# SPDX-FileCopyrightText: 2019-2023 Laurent Montel <montel@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

set(KDE_INSTALL_INCLUDEDIR_PIM ${KDE_INSTALL_INCLUDEDIR}/KPim)
add_library(KPimPkPass)
add_library(KPim::PkPass ALIAS KPimPkPass)

target_sources(KPimPkPass PRIVATE
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

ecm_qt_declare_logging_category(KPimPkPass
        HEADER logging.h
        IDENTIFIER KPkPass::Log
        CATEGORY_NAME org.kde.pkpass
        DESCRIPTION "KPkPass" EXPORT KPKPASS)


generate_export_header(KPimPkPass BASE_NAME KPkPass)
set_target_properties(KPimPkPass PROPERTIES
    VERSION ${KPKPASS_VERSION}
    SOVERSION ${KPKPASS_SOVERSION}
    EXPORT_NAME PkPass
)
target_include_directories(KPimPkPass INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_PIM}>")
target_link_libraries(KPimPkPass
    PUBLIC
        Qt::Gui
    PRIVATE
        KF${KF_MAJOR_VERSION}::Archive
)

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KPimPkPass PROPERTIES UNITY_BUILD ON)
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

install(TARGETS KPimPkPass EXPORT KPimPkPassTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
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
        KPimPkPass_QCH
        NAME KPimPkPass
        BASE_NAME KPimPkPass
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${KPkPass_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        #IMAGE_DIRS "${CMAKE_SOURCE_DIR}/docs/pics"
        LINK_QCHS
            Qt5Core_QCH
            Qt5Gui_QCH
            Qt5Widgets_QCH
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
