#ifndef MC_TEST_MESSAGE_UDP_COMMON_H_
#define MC_TEST_MESSAGE_UDP_COMMON_H_

#include <stdint.h>
#include <stdbool.h>

#define SERVER_PORT       9000
#define CLIENT_PORT       9001
#define COMPLETE_COUNT    1000
#define DATA_LEN          10

extern volatile bool Error;

uint32_t socket_write(int socket_fd, const void* data, uint32_t size, char* const dst_ip, uint16_t dst_port);
uint32_t socket_read(int socket_fd, void* data, uint32_t size);


#endif /* MC_TEST_MESSAGE_UDP_COMMON_H_ */
