# Subdirectory


# Variables
set(LIB micro_core_io_message)
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(HEADERS 
    ${INCLUDE_DIRS}/io/message/message.h
)
set(SOURCES 
    message/message.c
)


# Binray
add_library(${LIB} STATIC ${SOURCES} ${HEADERS})


# Include
target_include_directories(${LIB} PUBLIC ${INCLUDE_DIRS})


# Linking
