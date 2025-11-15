cmake_minimum_required(VERSION 3.21..4.0)

project(Terrarium
    LANGUAGES C CXX
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

include(FetchContent)

add_subdirectory("terrarium-core")
add_subdirectory("terrarium")
