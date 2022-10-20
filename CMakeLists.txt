cmake_minimum_required(VERSION 3.14)

project(millijson
    VERSION 1.0.0
    DESCRIPTION "Lightweight JSON library for C++"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(millijson INTERFACE)

target_include_directories(millijson INTERFACE include)

# Building the test-related machinery, if we are compiling this library directly.
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(INSTALL_GTEST OFF)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif() 
endif()

