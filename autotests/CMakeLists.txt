find_package(Qt5Test ${QT_REQUIRED_VERSION} CONFIG REQUIRED)
add_definitions(-DSOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

ecm_add_test(pkpasstest.cpp LINK_LIBRARIES Qt5::Test KPim::PkPass)
