# Subdirectory
add_subdirectory(core)
add_subdirectory(dsa)
add_subdirectory(io)


# Variables
set(LIB mcore)
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include/io)
set(HEADERS ${INCLUDE_DIRS}/io.h)
set(SOURCES )


# Binray
add_library(${LIB} INTERFACE ${SOURCES} ${HEADERS})


# Include


# Linking
target_link_libraries(${LIB} 
  INTERFACE
  mcore_core
  mcore_io
)


