cmake_minimum_required(VERSION 3.21)
project(MergeSort C)

set(CMAKE_C_STANDARD 99)

add_executable(MergeSort main.c timer.h)
set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )