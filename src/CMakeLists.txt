aux_source_directory(./source SRC)

#include 
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}
    )

add_definitions(-D_CONSOLE -D_LIB -DHAVE_CONFIG_H -DHAVE_STRDUP)

option(BUILD_STATIC_LIB "is buid static lib" ON)
option(BUILD_SHARED_LIB "is buid shared lib" ON)

if (BUILD_STATIC_LIB)
    add_library( libqrencode_s STATIC ${SRC})
endif (BUILD_STATIC_LIB)

if (BUILD_SHARED_LIB)
    if(WIN32)
        set(SRC ${SRC} qrcodelib.def)     
    endif()

    add_library( libqrencode SHARED ${SRC})
endif (BUILD_SHARED_LIB)