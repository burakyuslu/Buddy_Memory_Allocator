cmake_minimum_required(VERSION 3.16)
project(untitled C)

set(CMAKE_C_STANDARD 99)

add_executable(untitled app.c create_memory_sb.c destroy_memory_sb.c sbmemlib.c)