# SPDX-FileCopyrightText: 2018-2020 Volker Krause <vkrause@kde.org>
# SPDX-FileCopyrightText: 2018-2022 Laurent Montel <montel@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(PIM_VERSION "5.20.41")
project(KPkPass VERSION ${PIM_VERSION})

set(KF5_MIN_VERSION "5.95.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(ECMGenerateHeaders)
include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEGitCommitHooks)
include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
include(KDEInstallDirs)
include(KDECMakeSettings)
include(GenerateExportHeader)
include(ECMAddQch)


option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

ecm_setup_version(PROJECT VARIABLE_PREFIX KPKPASS
    VERSION_HEADER kpkpass_version.h
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPimPkPassConfigVersion.cmake"
)

set(QT_REQUIRED_VERSION "5.15.2")

find_package(Qt${QT_MAJOR_VERSION} ${QT_REQUIRED_VERSION} REQUIRED COMPONENTS Gui)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Archive)
if (NOT ANDROID)
    find_package(SharedMimeInfo 1.3 REQUIRED)
endif()

ecm_set_disabled_deprecation_versions(QT 5.15.2 KF 5.95.0)



option(USE_UNITY_CMAKE_SUPPORT "Use UNITY cmake support (speedup compile time)" OFF)

set(COMPILE_WITH_UNITY_CMAKE_SUPPORT OFF)
if (USE_UNITY_CMAKE_SUPPORT)
    set(COMPILE_WITH_UNITY_CMAKE_SUPPORT ON)
endif()

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)


set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPimPkPass")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimPkPassConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPimPkPassConfig.cmake"
    INSTALL_DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/KPimPkPassConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/KPimPkPassConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

install(EXPORT KPimPkPassTargets
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE KPimPkPassTargets.cmake
        NAMESPACE KPim::
)

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KPimPkPass_QCH
        FILE KPimPkPassQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KPimPkPassQchTargets.cmake\")")
endif()
