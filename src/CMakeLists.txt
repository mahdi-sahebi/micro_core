# Subdirectory
add_subdirectory(core)
add_subdirectory(alg)
add_subdirectory(dsa)
add_subdirectory(io)


# Variables
set(LIB mcore)
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include/io)# TODO(MN): Must be general and not io
set(HEADERS ${INCLUDE_DIRS}/io.h)# TODO(MN): Must be general and not io
set(SOURCES )


# Binray
add_library(${LIB} INTERFACE ${SOURCES} ${HEADERS})


# Include


# Linking
target_link_libraries(${LIB} 
  INTERFACE
  mcore_core
  mcore_alg
  mcore_io
)


