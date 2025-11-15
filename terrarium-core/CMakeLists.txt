find_package(SDL3 CONFIG)
if(NOT SDL3_FOUND)
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
        GIT_TAG "release-3.2.26"
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(SDL3)
endif()

find_package(glm CONFIG)
if(NOT glm_FOUND)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY "https://github.com/g-truc/glm.git"
        GIT_TAG "1.0.1"
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(glm)
endif()

FetchContent_Declare(
    glad
    GIT_REPOSITORY "https://github.com/Dav1dde/glad.git"
    GIT_TAG "v2.0.8"
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR "cmake"
)

FetchContent_MakeAvailable(glad)
glad_add_library(glad_gl_core46 SHARED API gl:core=4.6 EXTENSIONS NONE)

set(TERRARIUM_CORE_SOURCE_DIR "src")
set(TERRARIUM_CORE_INCLUDE_DIR "include")

file(GLOB_RECURSE TERRARIUM_CORE_SOURCES CONFIGURE_DEPENDS
    "${TERRARIUM_CORE_SOURCE_DIR}/*.cpp"
    "${TERRARIUM_CORE_INCLUDE_DIR}/*.hpp"
)

add_library(terrarium-core ${TERRARIUM_CORE_SOURCES})

target_include_directories(terrarium-core
    PUBLIC
    "${TERRARIUM_CORE_INCLUDE_DIR}"
    PRIVATE
    "${TERRARIUM_CORE_SOURCE_DIR}"
)

target_link_libraries(terrarium-core
    PRIVATE
    SDL3::SDL3
    glm::glm
    glad_gl_core46
)

target_compile_features(terrarium-core
    PUBLIC
    cxx_std_23
)
