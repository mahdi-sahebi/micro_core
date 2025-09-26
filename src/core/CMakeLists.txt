# Subdirectory
add_subdirectory(time)


# Variables
set(LIB ${PROJECT_NAME}_core)


# Binary
add_library(${LIB} INTERFACE)


# Include


# Linking
target_link_libraries(${LIB} 
  INTERFACE
  ${PROJECT_NAME}_time
)


