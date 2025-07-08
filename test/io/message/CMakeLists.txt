# Options
option(BUILD_TEST_IO_MESSAGE "To compile tests" ON)
if (NOT BUILD_TEST_IO_MESSAGE)
  exit()
endif()

if (NOT UNIX)
  message(ERROR "Platform not supported. Only linux is suppported")
  exit()
endif()


# Variables
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(TEST test_message_udp)


# Subdirectory


# Binray
add_executable(${TEST} 
  udp/test_message_udp_common.c   udp/test_message_udp_common.h
  udp/test_message_udp_sender.c   udp/test_message_udp_sender.h
  udp/test_message_udp_receiver.c udp/test_message_udp_receiver.h
  udp/test_message_udp.c
)


# Include
target_include_directories(${TEST} PRIVATE ${INCLUDE_DIRS})


# Linking
target_link_libraries(${TEST} PRIVATE mcore_io)

