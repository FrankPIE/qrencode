#cmake version set
cmake_minimum_required(VERSION 3.0.2)

#project infomation
project (QREncode C)

#version number
set (QRENCODE_VERSION_MAJOR 3)
set (QRENCODE_VERSION_MINOR 4)
set (QRENCODE_VERSION_MICRO 4)

configure_file(
    "${PROJECT_SOURCE_DIR}/src/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

#include 
include_directories("${PROJECT_BINARY_DIR}/src")
include_directories("${PROJECT_BINARY_DIR}")

target_link_libraries(QREncode )