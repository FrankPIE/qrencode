aux_source_directory(. SRC)

#include 
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR})

add_definitions(-D_CONSOLE -D_LIB -DHAVE_CONFIG_H -DHAVE_STRDUP)

add_library( libqrencode STATIC ${SRC})