# Subdirectory


# Variables
set(LIB micro_core_dsa)
set(PUBLIC_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include/)
set(HEADERS 
    ${PUBLIC_INCLUDE_DIRS}/dsa/span.h
    ${PUBLIC_INCLUDE_DIRS}/dsa/memory/memlin.h
    ${PUBLIC_INCLUDE_DIRS}/dsa/sarray.h
)
set(SOURCES 
    span.c
    memory/memlin.c
    sarray.c
)


# Binray
add_library(${LIB} STATIC ${SOURCES} ${HEADERS})


# Include
target_include_directories(${LIB} PUBLIC ${PUBLIC_INCLUDE_DIRS})


# Linking
