cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(PIM_VERSION "5.17.40")
project(KPkPass VERSION ${PIM_VERSION})

set(KF5_MIN_VERSION "5.80.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(ECMGenerateHeaders)
include(ECMQtDeclareLoggingCategory)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEGitCommitHooks)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(GenerateExportHeader)

ecm_setup_version(PROJECT VARIABLE_PREFIX KPKPASS
    VERSION_HEADER kpkpass_version.h
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPimPkPassConfigVersion.cmake"
)

set(QT_REQUIRED_VERSION "5.15.0")

find_package(Qt5 ${QT_REQUIRED_VERSION} REQUIRED COMPONENTS Gui)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Archive)
find_package(SharedMimeInfo 1.3 REQUIRED)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f00)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055200)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
