add_executable(${NAME} "main.c")

target_link_libraries(${NAME} server)
target_link_libraries(${NAME} util)
