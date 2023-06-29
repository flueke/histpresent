cmake_minimum_required(VERSION 3.19)
project(histpresent DESCRIPTION "HistPresent")

set(HISTPRESENT_IS_MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(HISTPRESENT_IS_MAIN_PROJECT ON)
endif()

# Create our binaries in the root of the build directory. It's just more convenient.
if (HISTPRESENT_IS_MAIN_PROJECT)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)
