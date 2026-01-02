cmake_minimum_required(VERSION 3.21..4.0)

project(Terrarium
    LANGUAGES C CXX
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

include(FetchContent)

option(TERRARIUM_BUILD_SHARED "Build a shared version of the library" OFF)
option(TERRARIUM_BUILD_STATIC "Build a static version of the library" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(TerrariumCommands)

if(NOT TERRARIUM_BUILD_SHARED AND NOT TERRARIUM_BUILD_STATIC)
    if(DEFINED BUILD_SHARED_LIBS)
        if(BUILD_SHARED_LIBS)
            set(TERRARIUM_BUILD_SHARED ON)
        else()
            set(TERRARIUM_BUILD_STATIC ON)
        endif()
    else()
        set(TERRARIUM_BUILD_SHARED ON)
    endif()
endif()

add_subdirectory("terrarium-core")
add_subdirectory("terrarium-platform")
add_subdirectory("terrarium")
