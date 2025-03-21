# Options
option(BUILD_TESTS "To compile tests" OFF)

# Variables
set(TEST ${PROJECT_NAME}_test)
set(HEADERS )
set(SOURCES )

# Binray
add_library(${LIB} STATIC ${SOURCES} ${HEADERS})

# Include

# Linking
