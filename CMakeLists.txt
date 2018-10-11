cmake_minimum_required(VERSION 3.0)
set(PIM_VERSION "5.9.3")
project(KPkPass VERSION ${PIM_VERSION})

find_package(ECM 5.38 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMAddTests)
include(ECMGenerateHeaders)
include(ECMQtDeclareLoggingCategory)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(GenerateExportHeader)

ecm_setup_version(PROJECT VARIABLE_PREFIX KPKPASS
    VERSION_HEADER kpkpass_version.h
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPimPkPassConfigVersion.cmake"
)

find_package(Qt5 REQUIRED COMPONENTS Test Gui)
find_package(KF5 REQUIRED COMPONENTS Archive)
find_package(SharedMimeInfo 1.3 REQUIRED)

add_definitions("-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII")
add_definitions(-DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT)
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)
add_definitions( -DQT_USE_QSTRINGBUILDER )

add_subdirectory(src)
add_subdirectory(autotests)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)


set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KPimPkPass")

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
install(FILES org_kde_kpkpass.categories DESTINATION ${KDE_INSTALL_CONFDIR})
