#include 
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}
    )

add_definitions(-D_CONSOLE -D_LIB -DHAVE_CONFIG_H -DHAVE_STRDUP)

add_executable(RenderWithOpenGL main.cpp)

if (BUILD_SHARED_LIB)
    target_link_libraries(RenderWithOpenGL libqrencode)
elseif(BUILD_STATIC_LIB)
    target_link_libraries(RenderWithOpenGL libqrencode_s)
endif()

