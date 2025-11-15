add_executable(terrarium "src/main.cpp")

target_link_libraries(terrarium
    PRIVATE
    terrarium-core
)

if(WIN32)
    add_custom_command(
        TARGET terrarium POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_RUNTIME_DLLS:terrarium>"
        "$<TARGET_FILE_DIR:terrarium>"
        COMMAND_EXPAND_LISTS
    )
endif()
