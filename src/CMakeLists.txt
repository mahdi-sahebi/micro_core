# Subdirectory
add_subdirectory(dsa)
add_subdirectory(io)


# Variables
set(LIB micro_core)
# set(HEADERS )
# set(SOURCES )


# Binray
add_library(${LIB} INTERFACE)


# Include


# Linking
target_link_libraries(${LIB} 
  INTERFACE
  micro_core_io
)


