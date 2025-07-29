
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "core/error.h"
#include "core/version.h"
#include "core/time.h"
#include "io/communication/communication.h"
#include "test_communication_udp_common.h"
#include "test_communication_udp_receiver.h"


static int ServerSocket = -1;
static uint32_t ReceiveCounter = 0;
static mc_time_t LastTickUS = 0;
static uint32_t* Result = NULL;
static mc_comm_t* message = NULL;
static mc_span AllocBuffer = {0};


static void server_create()
{
  ServerSocket = socket(AF_INET, SOCK_DGRAM, 0);
  
  struct sockaddr_in addr_in;
  memset(&addr_in, 0, sizeof(addr_in));
  addr_in.sin_family = AF_INET;
  addr_in.sin_port = htons(SERVER_PORT);
  addr_in.sin_addr.s_addr = INADDR_ANY;
  bind(ServerSocket, (struct sockaddr*)&addr_in, sizeof(addr_in));

  struct timeval timeout = {0, 10000};
  setsockopt(ServerSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

static uint32_t server_write(const void* const data, uint32_t size)
{
  return socket_write(ServerSocket, data, size, "127.0.0.1", CLIENT_PORT);
}

static uint32_t server_read(void* const data, uint32_t size)
{
  return socket_read(ServerSocket, data, size);
}
static void server_close()
{
  close(ServerSocket);
}

static void flush_receive_buffer()
{
  char temp[100];
  for (uint32_t index = 0; index < 10; index++) {
    server_read(temp, sizeof(temp));
  }
}

static bool verify_data(const uint32_t* const buffer, uint32_t packet_id) 
{
  for (uint32_t index = 0; index < DATA_LEN; index++) {
    const uint32_t expected = (packet_id * DATA_LEN) + index;
    if (expected != buffer[index]) {
        printf("Error - Packet #%u - got %u, expected %u\n", index, buffer[index], expected);
        return false;
    }
  }

  return true;
}

static void print_progress(float progress)
{
  const uint32_t BAR_LENGTH = 20;
  const uint32_t num_bars = progress * BAR_LENGTH;

  printf("\r\t\t\t\t\t\t\r");

  for (uint32_t index = 0; index < BAR_LENGTH; index++) {
    if (index < num_bars) {
      printf("█");
    } else {
      printf("▒");
    }
  }

  printf(" %.1f%%", progress * 100);
  if (1.0F == progress) {
    printf("\n");
  }
  fflush(stdout);
}

static void on_receive(const void* const data, uint32_t size)
{
  if ((DATA_LEN * sizeof(uint32_t)) != size) {
    *Result = MC_ERR_RUNTIME;
    return;
  }

  const uint32_t* const buffer = (uint32_t*)data;

  if (!verify_data(buffer, ReceiveCounter)) {
    *Result = MC_ERR_RUNTIME;
    return;
  }

  ReceiveCounter++;
  print_progress(ReceiveCounter / (float)cfg_get_iterations());
  LastTickUS = mc_now_u();
}

static void init(void* data)
{
  /* TODO(MN): Repetitive packet, invalid header, incomplete packet, miss packet pointer, use zero copy
   */
  Result = (uint32_t*)data;
  *Result = MC_SUCCESS;

  server_create();
  flush_receive_buffer();

  const uint32_t window_size = 16 + DATA_LEN * sizeof(uint32_t);
  const uint32_t window_capacity = 3;
  const uint32_t alloc_size = mc_comm_get_alloc_size(window_size, window_capacity).value;
  AllocBuffer = mc_span(malloc(alloc_size), alloc_size);

  message = mc_comm_init(AllocBuffer, window_size, window_capacity, mc_io(server_read, server_write), on_receive);

  ReceiveCounter = 0;
  LastTickUS = mc_now_u();
}

static void deinit()
{
  server_close();
  free(AllocBuffer.data);
}

static bool timed_out()
{
  return ((mc_now_u() - LastTickUS) > TEST_TIMEOUT_US);
}

static void wait_for_sender()
{
  const mc_time_t end_time = mc_now_u() + TEST_TIMEOUT_US;

  while (mc_now_u() < end_time) {
    mc_comm_recv(message);
  }
}

void* rcv_start(void* data)
{
  init(data);

  while (ReceiveCounter < cfg_get_iterations()) {
    if (timed_out()) {
      *Result = MC_ERR_TIMEOUT;
      break;
    }

    mc_comm_recv(message);
  }

  if ((MC_SUCCESS == *Result) && !mc_comm_flush(message, TEST_TIMEOUT_US)) {
    printf("mc_comm_flush failed\n");
    *Result = MC_ERR_TIMEOUT;
  }

  wait_for_sender();
  deinit();
  return NULL;
}
