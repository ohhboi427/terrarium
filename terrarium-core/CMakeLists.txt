terrarium_add_module(Core)
terrarium_glob_sources(${TERRARIUM_CORE_TARGET}
    "src/*.cpp"
    "src/core/*.cpp"
    "src/core/debug/*.cpp"
    "src/core/ecs/*.cpp"
    "src/core/utils/*.cpp"
)

if(WIN32)
    terrarium_glob_sources(${TERRARIUM_CORE_TARGET}
        "src/core/win/*.cpp"
        "src/core/debug/win/*.cpp"
    )
endif()

if(UNIX)
    terrarium_glob_sources(${TERRARIUM_CORE_TARGET}
        "src/core/linux/*.cpp"
        "src/core/debug/linux/*.cpp"
    )
endif()

target_include_directories(${TERRARIUM_CORE_TARGET}
    PUBLIC
    "include"
    PRIVATE
    "src"
)

target_compile_features(${TERRARIUM_CORE_TARGET}
    PUBLIC
    cxx_std_23
)

set_target_properties(${TERRARIUM_CORE_TARGET}
    PROPERTIES
    OUTPUT_NAME "libterrarium"
)
