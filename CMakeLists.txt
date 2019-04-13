cmake_minimum_required(VERSION 3.13)
project(unescape C)

set(CMAKE_C_STANDARD 99)

add_executable(unescape_pipe unescape_pipe.c unescape.c)
