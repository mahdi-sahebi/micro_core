#ifndef MC_TEST_MESSAGE_UDP_COMMON_H_
#define MC_TEST_MESSAGE_UDP_COMMON_H_

#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT       9000
#define CLIENT_PORT       9001
#define COMPLETE_COUNT    10000
#define DATA_LEN          10
#define TEST_TIMEOUT      5000000


uint32_t socket_write(int socket_fd, const void* data, uint32_t size, char* const dst_ip, uint16_t dst_port);
uint32_t socket_read(int socket_fd, void* data, uint32_t size);

uint32_t TimeNowU();


#endif /* MC_TEST_MESSAGE_UDP_COMMON_H_ */
