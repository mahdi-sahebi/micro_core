# Subdirectory


# Variables
set(LIB ${PROJECT_NAME}_dsa)
set(PUBLIC_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include/)
set(HEADERS 
    ${PUBLIC_INCLUDE_DIRS}/dsa/memory/memlin.h
    ${PUBLIC_INCLUDE_DIRS}/dsa/sarray.h
)
set(SOURCES 
    memory/memlin.c
    sarray.c
)


# Binray
add_library(${LIB} STATIC ${SOURCES} ${HEADERS})


# Include
target_include_directories(${LIB}
    PUBLIC ${PUBLIC_INCLUDE_DIRS}
)


# Linking
target_link_libraries(${LIB}
    PUBLIC ${PROJECT_NAME}_alg
)

