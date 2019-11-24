cmake_minimum_required(VERSION 3.5)
set(PIM_VERSION "5.12.90")
project(KPkPass VERSION ${PIM_VERSION})

set(KF5_MIN_VERSION "5.63.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

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

set(QT_REQUIRED_VERSION "5.11.0")

find_package(Qt5 ${QT_REQUIRED_VERSION} REQUIRED COMPONENTS Test Gui)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Archive)
find_package(SharedMimeInfo 1.3 REQUIRED)
option(NO_REGENERATE_MIME "Don't regenerate mime file (only for developper)" FALSE )

if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
   add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)
   add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x060000)
endif()

add_definitions(-DQT_NO_FOREACH)


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
install(FILES org_kde_kpkpass.categories  DESTINATION  ${KDE_INSTALL_LOGGINGCATEGORIESDIR})
