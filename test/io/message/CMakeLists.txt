# Options
if (NOT UNIX)
  message(ERROR "Platform not supported. Only linux is suppported")
  exit()
endif()


# Variables
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(TEST test_message_udp)
set(HEADERS 
  test_common.h
  test_receiver.h
  test_sender.h
)
set(SOURCES
  udp/test_common.c 
  udp/test_receiver.c
  udp/test_sender.c 
  udp/test_message.c
)


# Subdirectory


# Binary
add_executable(${TEST} ${HEADERS} ${SOURCES})


# Include
target_include_directories(${TEST} PRIVATE ${INCLUDE_DIRS})


# Linking
target_link_libraries(${TEST} PRIVATE ${PROJECT_NAME}_time ${PROJECT_NAME}_msg)

