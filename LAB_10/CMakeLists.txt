cmake_minimum_required(VERSION 3.22)
project(LAB_10 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

add_executable(LAB_10 main.c)
