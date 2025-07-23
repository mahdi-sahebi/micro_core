# Options


# Variables
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(HEADERS 
)
set(SOURCES 
  test_alg.c
)
set(TEST test_alg)


# Subdirectory


# Binray
add_executable(${TEST} ${HEADERS} ${SOURCES})


# Include


# Linking
target_link_libraries(${TEST}
  PRIVATE mcore_alg
)

