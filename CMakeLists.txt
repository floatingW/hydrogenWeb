# Works with 3.14 and versions above are OK
cmake_minimum_required(VERSION 3.14...3.16)

# project
project(hydrogenWeb
        VERSION 0.1
        DESCRIPTION "A very light web server"
        LANGUAGES CXX)

# Imports cmake modules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# for main project
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(default_build_type "Release")
    if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
        set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
                STRING "Choose the type of build." FORCE)
    endif ()

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

    # find spdlog
    find_package(spdlog REQUIRED)

    # find SQLite3
    find_package(SQLite3 REQUIRED)

endif ()

# Need fetchcontent for some extra modules
include(FetchContent)

# Adds subdirectory
add_subdirectory(src)
add_subdirectory(application)
