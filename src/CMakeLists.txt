set(TERRARIUM_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(TERRARIUM_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

file(GLOB_RECURSE TERRARIUM_SOURCES CONFIGURE_DEPENDS
    "${TERRARIUM_SOURCE_DIR}/*.cpp"
    "${TERRARIUM_INCLUDE_DIR}/*.hpp"
)

add_executable(terrarium ${TERRARIUM_SOURCES})

target_include_directories(terrarium
    PRIVATE
    "${TERRARIUM_INCLUDE_DIR}"
)

target_compile_features(terrarium
    PUBLIC
    cxx_std_23
)
