cmake_minimum_required(VERSION 3.24)
project(sish C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(sish
        sish.c data-structures.h flags-parser.c flags-parser.h shell-builtins.c shell-builtins.h command-parser.c command-parser.h)
