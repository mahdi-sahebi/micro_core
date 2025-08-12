# Subdirectory


# Variables
set(LIB mcore_io)
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(HEADERS 
    ${INCLUDE_DIRS}/io/communication/window.h
    ${INCLUDE_DIRS}/io/communication/window_pool.h
    ${INCLUDE_DIRS}/io/communication/communication.h
)
set(SOURCES 
    communication/window_pool.c
    communication/communication.c
)


# Binray
add_library(${LIB} STATIC ${SOURCES} ${HEADERS})


# Include
target_include_directories(${LIB} 
    PUBLIC ${INCLUDE_DIRS}
)


# Linking
target_link_libraries(${LIB}
    PRIVATE ${PROJECT_NAME}_time ${PROJECT_NAME}_alg ${PROJECT_NAME}_pattern
)
