# Options
option(BUILD_TEST_IO "To compile tests" ON)
if (NOT BUILD_TEST_IO)
  exit()
endif()


# Variables


# Subdirectory
add_subdirectory(communication)
add_subdirectory(message)


# Binary


# Include


# Linking

