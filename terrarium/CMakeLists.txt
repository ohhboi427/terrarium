add_executable(terrarium "src/main.cpp")

target_link_libraries(terrarium
    PRIVATE
    Terrarium::Core
)

if(WIN32)
    add_custom_target(copy_runtime_dlls
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_RUNTIME_DLLS:terrarium>"
        "$<TARGET_FILE_DIR:terrarium>"
        COMMAND_EXPAND_LISTS
        DEPENDS "$<TARGET_RUNTIME_DLLS:terrarium>"
    )

    add_dependencies(terrarium copy_runtime_dlls)
endif()
