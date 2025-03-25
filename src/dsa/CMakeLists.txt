# Subdirectory

# Variables
set(LIB micro_core_dsa)
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(HEADERS ${INCLUDE_DIRS}/dsa/span.h)
set(SOURCES span.c)

# Binray
add_library(${LIB} STATIC ${SOURCES} ${HEADERS})

# Include
target_include_directories(${LIB} PRIVATE ${INCLUDE_DIRS})

# Linking
