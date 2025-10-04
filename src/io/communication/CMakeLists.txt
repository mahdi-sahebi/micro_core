# Subdirectory


# Variables
set(LIB ${PROJECT_NAME}_comm)
set(PUBLIC_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(HEADERS 
    ${PUBLIC_INCLUDE_DIRS}/io/communication/mc_communication.h
    mc_base.h
    mc_window.h
    mc_window_pool.h
    mc_io.h
    mc_frame.h
    mc_protocol.h
)
set(SOURCES 
    mc_window_pool.c
    mc_io.c
    mc_frame.c
    mc_protocol.c
    mc_communication.c
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
