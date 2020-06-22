# Works with 3.14 and versions above are OK
cmake_minimum_required(VERSION 3.14...3.16)

# project
project(hydrogenWeb
        VERSION 0.1
        DESCRIPTION "A very light web server"
        LANGUAGES CXX)

# Imports cmake modules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Sets compile flags
set(CXX_FLAGS
        -Wall
        -Wextra
        -Werror
        -pedantic
        -pedantic-errors
        )

# for main project
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # cxx standard

    # ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Testing only available if this is in the main application
    enable_testing()
    find_package(GTest REQUIRED)
    if (GTEST_FOUND)
        message(STATUS "found gtest")
        add_subdirectory(tests)
    else ()
        message(STATUS "GTest not found, not building tests")
    endif ()

    # find pthread
    find_package(Threads REQUIRED)

endif ()

# Need fetchcontent for some extra modules
include(FetchContent)

# Adds subdirectory
add_subdirectory(src)
add_subdirectory(application)
