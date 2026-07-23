cmake_minimum_required(VERSION 3.2.0)

project(test_framebuffer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(test_framebuffer
    framebuffer.c
    test_framebuffer.c
)

