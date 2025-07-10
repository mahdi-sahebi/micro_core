#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "test_message_udp_common.h"


static uint32_t TestIterations = COMPLETE_COUNT;
static bool RepetitiveSendEnable = false;
static uint8_t LossRate = 0;


static bool simulate_loss() 
{
  return ((random() / (float)RAND_MAX) * 100) < LossRate;
}

uint32_t socket_write(int socket_fd, const void* data, uint32_t size, char* const dst_ip, uint16_t dst_port)
{
  if (simulate_loss()) {
    return 0;// TODO(MN): Add delay
  }

  uint8_t count = RepetitiveSendEnable ? 2 : 1;

  struct sockaddr_in addr_in;
  memset(&addr_in, 0, sizeof(addr_in));
  addr_in.sin_family = AF_INET;
  addr_in.sin_port = htons(dst_port);
  addr_in.sin_addr.s_addr = inet_addr(dst_ip);

  socklen_t addr_len = sizeof(addr_in);

  uint32_t sent_size = 0;
  while (count) {
    sent_size = sendto(socket_fd, data, size, 0, (struct sockaddr*)&addr_in, addr_len);

    if (-1 == sent_size) {
      sent_size = 0;
    }
    if (sent_size == size) {
      count--;
    }
  }

  return sent_size;
}

uint32_t socket_read(int socket_fd, void* data, uint32_t size)
{
  if (simulate_loss()) {
    return 0;
  }
  
  struct sockaddr_in addr_in;
  socklen_t addr_len = sizeof(addr_in);
  uint32_t read_size = recvfrom(socket_fd, data, size, 0, (struct sockaddr*)&addr_in, &addr_len);

  char sender_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &addr_in.sin_addr, sender_ip, INET_ADDRSTRLEN);
  const uint16_t sender_port = ntohs(addr_in.sin_port);

  if (-1 == read_size) {
      read_size = 0;
  }
  
  return read_size;
}

void cfg_set_repetitive_send(bool enable)
{
  RepetitiveSendEnable = enable;
}

void cfg_set_loss_rate(uint8_t rate)
{
  LossRate = rate;
}

void cfg_set_iterations(uint32_t iterations)
{
  TestIterations = iterations;
}

uint32_t cfg_get_iterations()
{
  return TestIterations;
}
