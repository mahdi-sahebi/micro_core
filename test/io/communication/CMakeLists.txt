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
set(TEST test_comm_udp)


# Subdirectory


# Binray
add_executable(${TEST} 
  udp/test_common.c   udp/test_common.h
  udp/test_sender.c   udp/test_sender.h
  udp/test_receiver.c udp/test_receiver.h
  udp/test.c
)


# Include
target_include_directories(${TEST} PRIVATE ${INCLUDE_DIRS})


# Linking
target_link_libraries(${TEST} PRIVATE mcore_comm)

