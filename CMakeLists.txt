cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(micro_core LANGUAGES C VERSION 1.0.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(test)
