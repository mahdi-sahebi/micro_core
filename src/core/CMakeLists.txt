# Subdirectory
add_subdirectory(time)


# Variables
set(LIB micro_core_core)


# Binray
add_library(${LIB} INTERFACE)


# Include


# Linking
target_link_libraries(${LIB} 
  INTERFACE
  micro_core_time
)


