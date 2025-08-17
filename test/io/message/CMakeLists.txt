# Options
if (NOT UNIX)
  message(ERROR "Platform not supported. Only linux is suppported")
  exit()
endif()


# Variables
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
set(TEST test_message_udp)


# Subdirectory


# Binary
add_executable(${TEST} 
  udp/test_message.c
)


# Include
target_include_directories(${TEST} PRIVATE ${INCLUDE_DIRS})


# Linking
target_link_libraries(${TEST} PRIVATE mcore_time mcore_msg)

