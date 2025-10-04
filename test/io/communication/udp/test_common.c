#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "core/time.h"
#include "core/error.h"
#include "test_common.h"


typedef struct
{
  mc_time_t last_time;
  uint32_t  duration_ms;
  bool      is_connected;
}periodic_connection_t;

static uint32_t TestIterations = COMPLETE_COUNT;
static uint32_t TimeOutUS = TEST_TIMEOUT_US;
static char SendBuffer[8 * 1024];
static bool RepetitiveSendEnable = false;
static periodic_connection_t Periodic = {.is_connected = true, .duration_ms = 0, .last_time = 0};
static uint64_t LastRecvTime = 0;
static uint8_t LossRate = 0;
static uint32_t RecvCounter = 0;
static uint32_t SendCounter = 0;
static uint32_t RecvFailedCounter = 0;
static uint32_t SendFailedCounter = 0;


static bool simulate_loss() 
{
  return ((random() / (float)RAND_MAX) * 100) < LossRate;
}

static uint32_t base_socket_write(int socket_fd, cvoid* data, uint32_t size, char* const dst_ip, uint16_t dst_port)
{
  usleep(171);

  struct sockaddr_in addr_in = {
    .sin_family      = AF_INET,
    .sin_port        = htons(dst_port),
    .sin_addr.s_addr = inet_addr(dst_ip)
  };
  socklen_t addr_len = sizeof(addr_in);

  uint32_t sent_size = sendto(socket_fd, data, size, 0, (struct sockaddr*)&addr_in, addr_len);
  if (-1 == sent_size) {
    sent_size = 0;
  }

  return sent_size;
}

static uint32_t base_socket_read(int socket_fd, void* data, uint32_t size, char src_ip[INET_ADDRSTRLEN], uint16_t* src_port)
{
  usleep(159);
  
  struct sockaddr_in addr_in = {0};
  socklen_t addr_len = sizeof(addr_in);
  uint32_t read_size = recvfrom(socket_fd, data, size, 0, (struct sockaddr*)&addr_in, &addr_len);

  if (-1 == read_size) {
    read_size = 0;

    if (NULL != src_port) {
      *src_port = 0;
      memset(src_ip, 0x00, INET_ADDRSTRLEN);
    }
  } else {
    if (NULL != src_port) {
      inet_ntop(AF_INET, &addr_in.sin_addr, src_ip, INET_ADDRSTRLEN);
      *src_port = ntohs(addr_in.sin_port);
    }
  }
  
  return read_size;
}

uint32_t socket_write(int socket_fd, cvoid* data, uint32_t size, char* const dst_ip, uint16_t dst_port)
{   
  if (!Periodic.is_connected) {
    return 0;
  }

  uint8_t count = RepetitiveSendEnable ? 2 : 1;
  cvoid* data_buffer = data;

  if (simulate_loss()) {
    static bool packetDrop = false;// TODO(MN): static variables are being used for both client and server
    packetDrop = !packetDrop;
    SendFailedCounter++;

    if (packetDrop) { /* Drop the packet */
      SendCounter++;
      return 0;
    } else {          /* Packet corruption */
      memcpy(SendBuffer, data, size);
      data_buffer = SendBuffer;
      SendBuffer[29] ^= 1;
    }
  }

  uint32_t sent_size = 0;

  while (count--) {
    SendCounter++;
    sent_size = base_socket_write(socket_fd, data_buffer, size, dst_ip, dst_port);
  }

  return sent_size;
}

uint32_t socket_read(int socket_fd, void* data, uint32_t size)
{
  // TODO(MN): Tests of droping a small part of receiving packet
  uint32_t read_size = base_socket_read(socket_fd, data, size, NULL, NULL);

  if (0 != Periodic.duration_ms) {
    if ((mc_now_m() - Periodic.last_time) > Periodic.duration_ms) {
      Periodic.last_time = mc_now_m();
      Periodic.is_connected = !Periodic.is_connected;
    }
    
    if (!Periodic.is_connected) {
      return 0;
    }
  }

// TOOD(MN): Complete log: # CRC failed, # packet dropped, #memory dropped, # max disconnection duratin 
// #Recv FIFO clear

  RecvCounter++;
  static bool bit_corruption = false;
  bool packetDrop = false;

  if (simulate_loss()) {
    packetDrop = true;
    bit_corruption = !bit_corruption;
    RecvFailedCounter++;
    
    if (!bit_corruption) {
      return 0;
    }
  }
  
  if (0 != read_size) {
    if (packetDrop && bit_corruption) {
      ((uint8_t*)data)[29] ^= 1;
    }
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

void cfg_set_periodic_duration(uint32_t duration_ms)
{
  Periodic.duration_ms = duration_ms;
}

uint32_t cfg_get_periodic_duration()
{
  return Periodic.duration_ms;
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

void cfg_set_timeout_us(uint32_t timeout_us)
{
  TimeOutUS = timeout_us;
}

uint32_t cfg_get_timeout_us()
{
  return TimeOutUS;
}
