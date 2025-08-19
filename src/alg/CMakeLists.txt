# Subdirectory


# Variables
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(HEADERS 
  ${INCLUDE_DIRS}/alg/mc_buffer.h
  ${INCLUDE_DIRS}/alg/algorithm.h
)
set(SOURCES 
  algorithm.c
)
set(LIB ${PROJECT_NAME}_alg)


# Binray
add_library(${LIB} STATIC ${SOURCES} ${HEADERS})


# Include
target_include_directories(${LIB} 
  PUBLIC ${INCLUDE_DIRS}
)


# Linking
