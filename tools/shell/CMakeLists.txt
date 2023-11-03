set(SHELL_SOURCES shell.cpp)
add_executable(shell ${SHELL_SOURCES})

target_link_libraries(shell stronk)
set_target_properties(shell PROPERTIES OUTPUT_NAME stronk-shell)