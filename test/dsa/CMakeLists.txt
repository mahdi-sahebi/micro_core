# Options


# Variables
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(HEADERS 
)
set(SOURCES 
  test_sorted_array.c
)
set(TEST test_dsa)


# Subdirectory


# Binary
add_executable(${TEST} ${HEADERS} ${SOURCES})


# Include


# Linking
target_link_libraries(${TEST}
  PRIVATE ${PROJECT_NAME}_time ${PROJECT_NAME}_dsa
)

