
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "core/error.h"
#include "core/version.h"
#include "core/time.h"
#include "io/communication/communication.h"
#include "test_common.h"
#include "test_receiver.h"


static int ServerSocket = -1;
static uint32_t* Result = NULL;
static mc_comm* message = NULL;
static char TempBuffer[5 * 1024] = {0};
static mc_buffer AllocBuffer = mc_buffer(TempBuffer, sizeof(TempBuffer));
static mc_time_t BeginTime = 0;
static mc_time_t EndTime = 0;


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

  int recv_buf_size = 1460;
  if (setsockopt(ServerSocket, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, sizeof(recv_buf_size)) < 0) {
    perror("setsockopt SO_RCVBUF failed");
    close(ServerSocket);
    exit(EXIT_FAILURE);
  }
}

static uint32_t server_write(cvoid* const data, uint32_t size)
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

static void print_progress(float progress)
{
  cuint32_t BAR_LENGTH = 20;
  cuint32_t num_bars = progress * BAR_LENGTH;

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

static bool init(void* data)
{
  Result = (uint32_t*)data;
  *Result = MC_SUCCESS;

  server_create();
  flush_receive_buffer();
  memset(TempBuffer, 0x00, sizeof(TempBuffer));

  const mc_comm_cfg config = mc_comm_cfg(mc_io(server_read, server_write),
    mc_comm_wnd(1157, 3), mc_comm_wnd(59, 1));
    
  const mc_u32 result_u32 = mc_comm_get_alloc_size(config);
  if (MC_SUCCESS != result_u32.error) {
    *Result = result_u32.error;
    return false;
  }
  if (AllocBuffer.capacity < result_u32.value) {
    printf("[Receive] Not enough space for test\n");
    *Result = MC_ERR_OUT_OF_RANGE;
    return false;
  }
  
  AllocBuffer.capacity = result_u32.value;
  const mc_ptr result = mc_comm_init(AllocBuffer, config);
  if (MC_SUCCESS != result.error) {
    *Result = result.error;
    return false;
  }

  message = result.data;
  BeginTime = mc_now();
  return true;
}

static void print_log()
{
  const float duration_s = (EndTime - BeginTime) / 1000000000.0F;
  cuint32_t size_1 = 9 * sizeof(char);
  cuint32_t size_2 = 1024 * sizeof(uint32_t);
  cuint32_t size_3 = 1 * sizeof(bool);
  cuint32_t size_k_byte_ps = (size_1 + size_2 + size_3) * cfg_get_iterations() / 1024;
  const float throughput = size_k_byte_ps / duration_s;

  cuint32_t recv_cnt = cfg_get_recv_counter();
  cuint32_t send_cnt = cfg_get_send_counter();
  cuint32_t recv_failed_cnt = cfg_get_recv_failed_counter();
  cuint32_t send_failed_cnt = cfg_get_send_failed_counter();
  printf("[IO] Completed{Recv: %u, Send: %u} - Failed{Recv: %u(%.2f%%), Send: %u(%.2f%%)} - Throughput: %.2f KBps\n",
      recv_cnt, send_cnt, 
      recv_failed_cnt, 100 * (recv_failed_cnt / (float)recv_cnt),
      send_failed_cnt, 100 * (send_failed_cnt / (float)send_cnt),
      throughput
    );
}

static void deinit()
{
  server_close();
  print_log();
}

static void wait_for_sender()
{
  EndTime = mc_now();
  const mc_time_t end_time = mc_now_m() + (cfg_get_periodic_duration() * 1)  + (100 * cfg_get_loss_rate());

  while (mc_now_m() < end_time) {
    mc_comm_update(message);
  } 
}

static bool recv_data(void* data, uint32_t size)
{
  const mc_u32 result = mc_comm_recv(message, data, size, TEST_TIMEOUT_US);

  if((MC_SUCCESS != result.error) || (result.value != size)) {
    *Result = MC_ERR_TIMEOUT;
    return false;
  }

  return true;
}

static bool recv_string(uint32_t seed)
{
  char data[9] = {0};
  cuint32_t size = sizeof(data);
  
  if (!recv_data(data, size)) {
    printf("[ERR Data 1] Incomplete receiving\n");
    return false;
  }

  if (0 != memcmp(&data[0], "!p", 2)) {
    printf("[ERR Data 1] wrong data received\n");
    return false;
  }

  if (0 != memcmp(&data[5], ".?I", 3)) {
    printf("[ERR Data 1] wrong data received\n");
    return false;
  }

  char num_text[4] = {0};
  sprintf(num_text, "%03u", seed % 1000);
  if (0 != memcmp(num_text, &data[2], 3)) {
    printf("[ERR Data 1] wrong data received\n");
    return false;
  }

  return true;
}

static bool recv_variadic_size(uint32_t seed)
{
  uint32_t data[1024] = {0};
  cuint32_t random_count = (seed * 1664525) + 1013904223;
  cuint32_t count = (random_count % 997) + 27;
  cuint32_t size = count * sizeof(*data);

  if (!recv_data(data, size)) {
    printf("[ERR Data 2] Incomplete receiving\n");
    return false;
  }

  for (uint32_t index = 0; index < count; index++) {
    cuint32_t expected = ((index & 1) ? -56374141.31 : +8644397.79) * (index + 1) * (seed + 1) + index;
    if (data[index] != expected) {
      printf("[ERR Data 2] Received: %u, Expected: %u\n", data[index], expected);
      return false;
    }
  }

  return true;
}

static bool recv_tiny_size(uint32_t seed)
{
  bool data = false;

  if (!recv_data(&data, sizeof(data))) {
    printf("[ERR Data 3] Incomplete receiving\n");
    return false;
  }

  if ((seed & 1) != data) {
    printf("[ERR Data 3] wrong data received\n");
    return false;
  }

  return true;
}

void* rcv_start(void* data)
{
  if (!init(data)) {
    return NULL;
  }

  for (uint32_t counter = 0; counter <= cfg_get_iterations(); counter++) {
    if (MC_SUCCESS != mc_comm_update(message)) {
      *Result = MC_ERR_TIMEOUT;
      break;
    }

    if (!recv_string(counter)        || 
        !recv_variadic_size(counter) || /* Smaller and larger than window size */
        !recv_tiny_size(counter)) {
      *Result = MC_ERR_TIMEOUT;
      break;
    }
    
    print_progress(counter / (float)cfg_get_iterations());
  }

  if (MC_SUCCESS == *Result) {
    const mc_bool result = mc_comm_flush(message, TEST_TIMEOUT_US);
    if ((MC_SUCCESS != result.error) || !result.value) {
      printf("mc_comm_flush failed\n");
      *Result = MC_ERR_TIMEOUT;
    }
  }

  wait_for_sender();
  deinit();
  return NULL;
}
