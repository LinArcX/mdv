add_executable(${NAME} "main.c" "style.c")

message(${CMAKE_BINARY_DIR})
message(${CMAKE_CURRENT_BINARY_DIR})

add_executable(embedfile embedfile.c)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/style.c
    COMMAND embedfile style ../assets/style.css
    DEPENDS ../assets/style.css
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")


target_link_libraries(${NAME} server)
target_link_libraries(${NAME} util)
