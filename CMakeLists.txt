cmake_minimum_required(VERSION 3.14)

project(millijson
    VERSION 2.0.0
    DESCRIPTION "Lightweight JSON library for C++"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(millijson INTERFACE)
add_library(ltla::millijson ALIAS millijson)

include(GNUInstallDirs)
target_include_directories(millijson INTERFACE 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ltla_millijson>")

# Building the test-related machinery, if we are compiling this library directly.
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(MILLIJSON_TESTS "Build millijson's test suite." ON)
else()
    option(MILLIJSON_TESTS "Build millijson's test suite." OFF)
endif()

if(MILLIJSON_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif() 
endif()

# Setting up the installation commands.
include(CMakePackageConfigHelpers)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ltla_millijson)

install(TARGETS millijson 
    EXPORT millijsonTargets)

install(EXPORT millijsonTargets
    FILE ltla_millijsonTargets.cmake
    NAMESPACE ltla::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_millijson)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/ltla_millijsonConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_millijson)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ltla_millijsonConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ltla_millijsonConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ltla_millijsonConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_millijson)
