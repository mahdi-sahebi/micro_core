# Subdirectory


# Variables
set(LIB mcore_pattern)
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(HEADERS 
    ${INCLUDE_DIRS}/pattern/mc_chain.h
)
set(SOURCES 
    mc_chain.c
)


# Binary
add_library(${LIB} STATIC ${SOURCES} ${HEADERS})


# Include
target_include_directories(${LIB} 
    PUBLIC ${INCLUDE_DIRS}
)


# Linking
target_link_libraries(${LIB}
)
