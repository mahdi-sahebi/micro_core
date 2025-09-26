# Subdirectory


# Variables
set(LIB ${PROJECT_NAME}_time)
set(PUBLIC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(HEADERS ${PUBLIC_INCLUDE_DIR}/core/time.h)
set(SOURCES "")
if (UNIX)
  set(SOURCES time_linux.c)
endif()


# Binary
add_library(${LIB} STATIC ${SOURCES} ${HEADERS})


# Include
target_include_directories(${LIB}
  PUBLIC ${PUBLIC_INCLUDE_DIR}
)


# Linking


