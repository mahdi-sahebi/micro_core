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


# Subdirectory


# Binray
add_executable(test_message_udp test_message_udp.c)


# Include
target_include_directories(test_message_udp PRIVATE ${INCLUDE_DIRS})


# Linking
# target_link_libraries(test_message_udp PRIVATE micro_io_message)

