add_library(
    util
    util/html.c
    util/stack.c
    util/buffer.c
    util/escape.c
    util/extern.h
    util/autolink.c
    util/document.c
    util/html_blocks.c)
target_include_directories(util PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/util")
