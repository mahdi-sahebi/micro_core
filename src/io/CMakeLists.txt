# Subdirectory


# Variables
set(LIB mcore_io)
set(PUBLIC_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/communication)
set(HEADERS 
    ${PUBLIC_INCLUDE_DIRS}/io/communication/window.h
    ${PUBLIC_INCLUDE_DIRS}/io/communication/window_pool.h
    ${PRIVATE_INCLUDE_DIRS}/mc_transceiver.h
    ${PRIVATE_INCLUDE_DIRS}/mc_frame.h
    ${PRIVATE_INCLUDE_DIRS}/mc_protocol.h
    ${PUBLIC_INCLUDE_DIRS}/io/communication/communication.h
)
set(SOURCES 
    communication/window_pool.c
    communication/mc_transceiver.c
    communication/mc_frame.c
    communication/mc_protocol.c
    communication/communication.c
)


# Binray
add_library(${LIB} STATIC ${SOURCES} ${HEADERS})


# Include
target_include_directories(${LIB} 
    PUBLIC ${PUBLIC_INCLUDE_DIRS}
)


# Linking
target_link_libraries(${LIB}
    PRIVATE ${PROJECT_NAME}_time ${PROJECT_NAME}_alg ${PROJECT_NAME}_pattern
)
