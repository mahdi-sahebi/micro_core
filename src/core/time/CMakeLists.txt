# Subdirectory


# Variables
set(LIB mcore_time)
set(PUBLIC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/core)
set(HEADERS ${PUBLIC_INCLUDE_DIR}/time.h)
if (UNIX)
  set(SOURCES time_linux.c)
endif()


# Binray
add_library(${LIB} INTERFACE ${SOURCES} ${HEADERS})


# Include


# Linking


