# Version
if (NOT DEFINED PROJECT_VERSION)
    set(PROJECT_VERSION "1.0.1")
endif()
if (NOT DEFINED PROJECT_VERSION_SUFFIX)
    set(PROJECT_VERSION_SUFFIX "-dev")
endif()


# Information
cmake_minimum_required(VERSION 3.1...4.1 FATAL_ERROR)
project(mcore LANGUAGES C VERSION ${PROJECT_VERSION})
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CPU Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Project Version: ${PROJECT_VERSION}${PROJECT_VERSION_SUFFIX}")


# Configuration
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage -O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -g0")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")


include(${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake/version.cmake)


# Sub-Directory
add_subdirectory(src)
add_subdirectory(test)
