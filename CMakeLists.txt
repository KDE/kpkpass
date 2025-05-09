# SPDX-FileCopyrightText: 2017-2020 Volker Krause <vkrause@kde.org>
# SPDX-FileCopyrightText: 2018-2025 Laurent Montel <montel@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(PIM_VERSION "6.4.2")
project(KPkPass VERSION ${PIM_VERSION})

set(KF_MIN_VERSION "6.9.0")
set(QT_REQUIRED_VERSION "6.7.0")
find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
set(CMAKE_CXX_STANDARD 20)

include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(ECMGenerateHeaders)
include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)
include(ECMFeatureSummary)
include(ECMSetupVersion)
include(KDEGitCommitHooks)
include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES
src/*.cpp
src/*.h
autotests/*.cpp
autotests/*.h
)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
include(KDEInstallDirs)
include(KDECMakeSettings)
include(ECMGenerateExportHeader)

include(ECMCheckOutboundLicense)
file(GLOB_RECURSE ALL_SOURCE_FILES
src/*.cpp
src/*.h
autotests/*.cpp
autotests/*.h
)
ecm_check_outbound_license(LICENSES GPL-2.0-only  FILES ${ALL_SOURCE_FILES})

include(ECMAddQch)


option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")


ecm_setup_version(PROJECT VARIABLE_PREFIX KPKPASS
    VERSION_HEADER src/kpkpass_version.h
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim6PkPassConfigVersion.cmake"
    SOVERSION 6
)

find_package(Qt6 ${QT_REQUIRED_VERSION} REQUIRED COMPONENTS Gui)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS Archive)
if(NOT ANDROID)
    find_package(SharedMimeInfo 1.3 REQUIRED)
endif()

add_definitions(-DQT_NO_CONTEXTLESS_CONNECT)
ecm_set_disabled_deprecation_versions(QT 6.9.0 KF 6.12.0)



option(USE_UNITY_CMAKE_SUPPORT "Use UNITY cmake support (speedup compile time)" OFF)

set(COMPILE_WITH_UNITY_CMAKE_SUPPORT OFF)
if(USE_UNITY_CMAKE_SUPPORT)
    set(COMPILE_WITH_UNITY_CMAKE_SUPPORT ON)
endif()

add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
ecm_feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)


set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim6PkPass")
if(BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KPim6PkPass_QCH
        FILE KPim6PkPassQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
   )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KPim6PkPassQchTargets.cmake\")")
endif()

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimPkPassConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6PkPassConfig.cmake"
    INSTALL_DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/KPim6PkPassConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/KPim6PkPassConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

install(EXPORT KPim6PkPassTargets
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE KPim6PkPassTargets.cmake
        NAMESPACE KPim6::
)

