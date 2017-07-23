#cmake version set
cmake_minimum_required(VERSION 3.0.2)

#project infomation
project (QREncode)

#version number
set (QRENCODE_VERSION_MAJOR 3)
set (QRENCODE_VERSION_MINOR 4)
set (QRENCODE_VERSION_MICRO 4)

configure_file(
    "${PROJECT_SOURCE_DIR}/src/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

option(BUILD_EXMPLES "Is build exmples" ON)

add_subdirectory(${PROJECT_SOURCE_DIR}/src)

if (BUILD_EXMPLES)
    add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
endif(BUILD_EXMPLES)