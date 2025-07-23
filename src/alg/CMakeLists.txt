# Subdirectory


# Variables
set(LIB mcore_alg)
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(HEADERS 
    ${INCLUDE_DIRS}/alg/algorithm.h
)
set(SOURCES 
    algorithm.c
)


# Binray
add_library(${LIB} STATIC ${SOURCES} ${HEADERS})


# Include
target_include_directories(${LIB} PRIVATE ${INCLUDE_DIRS})


# Linking
