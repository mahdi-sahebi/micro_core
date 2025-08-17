# Options


# Variables
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(HEADERS 
)
set(SOURCES 
  test_alg.c
)
set(TEST ${PROJECT_NAME}_test_alg)


# Subdirectory


# Binary
add_executable(${TEST} ${HEADERS} ${SOURCES})


# Include


# Linking
target_link_libraries(${TEST}
  PRIVATE ${PROJECT_NAME}_time ${PROJECT_NAME}_alg
)

