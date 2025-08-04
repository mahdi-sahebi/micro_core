#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "test_common.h"


static uint32_t TestIterations = COMPLETE_COUNT;
static char SendBuffer[1 * 1024];
static bool RepetitiveSendEnable = false;
static uint8_t LossRate = 0;
static uint32_t RecvCounter = 0;
static uint32_t SendCounter = 0;
static uint32_t RecvFailedCounter = 0;
static uint32_t SendFailedCounter = 0;


static bool simulate_loss() 
{
  return ((random() / (float)RAND_MAX) * 100) < LossRate;
}

uint32_t socket_write(int socket_fd, const void* data, uint32_t size, char* const dst_ip, uint16_t dst_port)
{
  uint8_t count = RepetitiveSendEnable ? 2 : 1;

  struct sockaddr_in addr_in = {
    .sin_family      = AF_INET,
    .sin_port        = htons(dst_port),
    .sin_addr.s_addr = inet_addr(dst_ip)
  };
  socklen_t addr_len = sizeof(addr_in);

  if (simulate_loss()) {
    static bool packetDrop = false;
    packetDrop = !packetDrop;

    if (packetDrop) { /* Drop the packet */
      SendFailedCounter++;
      return 0;
    } else {          /* Packet corruption */
      memcpy(SendBuffer, data, size);
      SendBuffer[29] ^= 1;
      sendto(socket_fd, SendBuffer, size, 0, (struct sockaddr*)&addr_in, addr_len);
      SendCounter++;
      return size;
    }
  }

  uint32_t sent_size = 0;
  while (count) {
    if (sendto(socket_fd, data, size, 0, (struct sockaddr*)&addr_in, addr_len) == size) {
      count--;
      SendCounter++;
    } else {
      sent_size = 0;
    }
  }

  return sent_size;
}

uint32_t socket_read(int socket_fd, void* data, uint32_t size)
{
  static bool bit_corruption = false;
  bool packetDrop = false;

  if (simulate_loss()) {
    packetDrop = true;
    bit_corruption = !bit_corruption;
    
    if (!bit_corruption) {
      RecvFailedCounter++;
      return 0;
    }
  }
  
  struct sockaddr_in addr_in = {0};
  socklen_t addr_len = sizeof(addr_in);
  uint32_t read_size = recvfrom(socket_fd, data, size, 0, (struct sockaddr*)&addr_in, &addr_len);

  char sender_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &addr_in.sin_addr, sender_ip, INET_ADDRSTRLEN);
  const uint16_t sender_port = ntohs(addr_in.sin_port);

  if (-1 == read_size) {
    read_size = 0;
  } else {
    if (packetDrop && bit_corruption) {
      ((uint8_t*)data)[29] ^= 1;
    }
    RecvCounter++;
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

uint8_t cfg_get_loss_rate()
{
  return LossRate;
}

void cfg_set_iterations(uint32_t iterations)
{
  TestIterations = iterations;
  RecvCounter = 0;
  SendCounter = 0;
  RecvFailedCounter = 0;
  SendFailedCounter = 0;
}

uint32_t cfg_get_iterations()
{
  return TestIterations;
}

uint32_t cfg_get_recv_counter()
{
  return RecvCounter;
}

uint32_t cfg_get_send_counter()
{
  return SendCounter;
}

uint32_t cfg_get_recv_failed_counter()
{
  return RecvFailedCounter;
}

uint32_t cfg_get_send_failed_counter()
{
  return SendFailedCounter;
}
