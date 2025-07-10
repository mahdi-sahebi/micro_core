# Options
option(BUILD_TESTS "To compile tests" ON)

# Variables
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)

# Subdirectory
add_subdirectory(io)

# Binray
add_executable(test_dsa_span test_dsa_span.c)
add_executable(test_dsa_memlin test_dsa_memlin.c)

# Include
target_include_directories(test_dsa_span PRIVATE ${INCLUDE_DIRS})
target_include_directories(test_dsa_memlin PRIVATE ${INCLUDE_DIRS})

# Linking
target_link_libraries(test_dsa_span PRIVATE micro_core_dsa)
target_link_libraries(test_dsa_memlin PRIVATE micro_core_dsa)

