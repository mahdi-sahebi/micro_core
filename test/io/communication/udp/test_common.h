#ifndef MC_TEST_MESSAGE_UDP_COMMON_H_
#define MC_TEST_MESSAGE_UDP_COMMON_H_

#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT       9000
#define CLIENT_PORT       9001
#define COMPLETE_COUNT    200
#define DATA_LEN          10
#define TEST_TIMEOUT_US   30000000


uint32_t socket_write(int socket_fd, const void* data, uint32_t size, char* const dst_ip, uint16_t dst_port);
uint32_t socket_read(int socket_fd, void* data, uint32_t size);

void     cfg_set_repetitive_send(bool enable);
void     cfg_set_loss_rate(uint8_t rate);
uint8_t  cfg_get_loss_rate();
void     cfg_set_iterations(uint32_t iterations);
uint32_t cfg_get_periodic_duration();
void     cfg_set_periodic_duration(uint32_t duration_ms);
uint32_t cfg_get_iterations();
uint32_t cfg_get_recv_counter();
uint32_t cfg_get_send_counter();
uint32_t cfg_get_recv_failed_counter();
uint32_t cfg_get_send_failed_counter();


#endif /* MC_TEST_MESSAGE_UDP_COMMON_H_ */
