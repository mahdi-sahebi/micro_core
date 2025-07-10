# Subdirectory
add_subdirectory(time)


# Variables
set(LIB mcore_core)


# Binray
add_library(${LIB} INTERFACE)


# Include


# Linking
target_link_libraries(${LIB} 
  INTERFACE
  mcore_time
)


