function(terrarium_glob_sources TARGET_NAME)
    file(GLOB SOURCES CONFIGURE_DEPENDS ${ARGN})

    target_sources(${TARGET_NAME}
        PRIVATE
        ${SOURCES}
    )
endfunction()

function(terrarium_add_module MODULE_NAME)
    string(TOLOWER ${MODULE_NAME} MODULE_LOWER)
    string(TOUPPER ${MODULE_NAME} MODULE_UPPER)

    if(TERRARIUM_BUILD_SHARED)
        set(SHARED_TARGET terrarium-${MODULE_LOWER}-shared)

        add_library(${SHARED_TARGET} SHARED)
        add_library(Terrarium::${MODULE_NAME} ALIAS ${SHARED_TARGET})

        target_compile_definitions(${SHARED_TARGET}
            PUBLIC
            TERRA_BUILD_SHARED
            PRIVATE
            TERRA_${MODULE_UPPER}_EXPORT
        )

        set(TERRARIUM_${MODULE_UPPER}_TARGET ${SHARED_TARGET} PARENT_SCOPE)
    endif()

    if(TERRARIUM_BUILD_STATIC)
        set(STATIC_TARGET terrarium-${MODULE_LOWER}-static)

        add_library(${STATIC_TARGET} STATIC)
        add_library(Terrarium::${MODULE_NAME} ALIAS ${STATIC_TARGET})

        set(TERRARIUM_${MODULE_UPPER}_TARGET ${STATIC_TARGET} PARENT_SCOPE)
    endif()
endfunction()
