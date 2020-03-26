message(${CMAKE_CURRENT_SOURCE_DIR})
message(${CMAKE_SOURCE_DIR})
add_library(
    util
    util/util.c
    util/util.h
    util/html.c
    util/stack.c
    util/buffer.c
    util/escape.c
    util/extern.h
    util/autolink.c
    util/document.c
    util/html_blocks.c)
target_include_directories(util PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/util")

add_library(
    server
    server/server.c
    server/server.h)
target_include_directories(server PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/server")
target_link_libraries(server PUBLIC util)
