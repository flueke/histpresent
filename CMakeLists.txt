cmake_minimum_required(VERSION 3.19)
project(histpresent DESCRIPTION "HistPresent")

# Create our binaries in the root of the build directory. It's just more convenient.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src)
