# glad library
# Source: https://gen.glad.sh/

add_library(glad_glad STATIC
    gl.cpp
)

target_include_directories(glad_glad PUBLIC include)
