# Options
option(BUILD_TESTS "To compile tests" ON)

# Variables
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(SOURCES test_dsa_span.c)

# Binray
add_executable(test_dsa_span test_dsa_span.c)

# Include
target_include_directories(test_dsa_span PRIVATE ${INCLUDE_DIRS})

# Linking
target_link_libraries(test_dsa_span PRIVATE micro_core_dsa)

