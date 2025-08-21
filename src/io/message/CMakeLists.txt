# Subdirectory


# Variables
set(LIB ${PROJECT_NAME}_msg)
set(PUBLIC_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(HEADERS 
  ${PUBLIC_INCLUDE_DIRS}/io/message/mc_message.h
)
set(SOURCES 
  mc_message.c
)


# Binary
add_library(${LIB} STATIC ${SOURCES} ${HEADERS})


# Include
target_include_directories(${LIB} 
  PRIVATE
  PUBLIC ${PUBLIC_INCLUDE_DIRS}
)


# Linking
target_link_libraries(${LIB}
  PRIVATE ${PROJECT_NAME}_dsa ${PROJECT_NAME}_comm
)
