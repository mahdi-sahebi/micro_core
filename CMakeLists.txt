cmake_minimum_required(VERSION 3.1...4.1 FATAL_ERROR)
project(mcore LANGUAGES C VERSION 1.0.1)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage -O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -g0")

add_subdirectory(src)
add_subdirectory(test)
