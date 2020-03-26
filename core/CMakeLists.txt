add_executable(${NAME} "main.c" "${CMAKE_CURRENT_BINARY_DIR}/style.c")

add_executable(embedfile embedfile.c)
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/style.c"
    COMMAND embedfile style "${CMAKE_SOURCE_DIR}/assets/style.css"
    DEPENDS "${CMAKE_SOURCE_DIR}/assets/style.css")

target_link_libraries(${NAME} server)
target_link_libraries(${NAME} util)
