# Subdirectory


# Variables
set(LIB ${PROJECT_NAME}_comm)
set(PUBLIC_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(HEADERS 
    ${PUBLIC_INCLUDE_DIRS}/io/communication/window.h
    ${PUBLIC_INCLUDE_DIRS}/io/communication/window_pool.h
    ${PUBLIC_INCLUDE_DIRS}/io/communication/communication.h
    mc_frame.h
)
set(SOURCES 
    window_pool.c
    mc_frame.c
    communication.c
)


# Binary
add_library(${LIB} STATIC ${SOURCES} ${HEADERS})


# Include
target_include_directories(${LIB} 
    PUBLIC ${PUBLIC_INCLUDE_DIRS}
)


# Linking
target_link_libraries(${LIB}
    PRIVATE ${PROJECT_NAME}_time ${PROJECT_NAME}_alg
)
