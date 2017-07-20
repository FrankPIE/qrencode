#include 
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}
    )

add_definitions(-D_CONSOLE -D_LIB -DHAVE_CONFIG_H -DHAVE_STRDUP)

add_executable(QREncodeExport main.cpp)

target_link_libraries(QREncodeExport libqrencode)