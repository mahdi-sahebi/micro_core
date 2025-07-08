# Subdirectory
add_subdirectory(core)
add_subdirectory(dsa)
add_subdirectory(io)


# Variables
set(LIB mcore)
# set(HEADERS )
# set(SOURCES )


# Binray
add_library(${LIB} INTERFACE)


# Include


# Linking
target_link_libraries(${LIB} 
  INTERFACE
  mcore_core
  mcore_io
)


