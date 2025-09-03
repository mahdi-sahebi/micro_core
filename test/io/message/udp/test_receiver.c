
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "core/error.h"
#include "core/version.h"
#include "core/time.h"
#include "io/message/mc_message.h"
#include "test_common.h"
#include "test_receiver.h"


static int ServerSocket = -1;
static char ClientIP[INET_ADDRSTRLEN];
static uint16_t ClientPort = 0;
static mc_error* Error = NULL;
static mc_msg* message = NULL;
static char AllocBuffer[2 * 1024];
static uint32_t TestCounter = 0;
static bool IsStringReceived = false;
static bool IsLargeReceived  = false;
static bool IsTinyReceived   = false;
static bool IsSignalReceived = false;
static mc_time_t BeginTime = 0;
static mc_time_t EndTime = 0;


static void server_create()
{
  ServerSocket = socket(AF_INET, SOCK_DGRAM, 0);
  
  int opt = 1;
  setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

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
  return socket_write(ServerSocket, data, size, ClientIP, ClientPort);
}

static uint32_t server_read(void* const data, uint32_t size)
{
  const uint32_t read_size = socket_read(ServerSocket, data, size, ClientIP, &ClientPort);
  if (strcmp(ClientIP, "127.0.0.1") || (0 == ClientPort)) {
    return 0;
  }
  return read_size;
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

static void on_string_received(mc_msg_id id, mc_buffer buffer)
{
  if ((77 != id) || mc_buffer_is_null(buffer) || (9 != mc_buffer_get_size(buffer))) {
    *Error = MC_ERR_RUNTIME;
    return;
  }
  
  if (0 != memcmp(&buffer.data[0], "!p", 2)) {
    printf("[ERR Data 1] wrong data received\n");
    *Error = MC_ERR_RUNTIME;
  }

  if (0 != memcmp(&buffer.data[5], ".?I", 3)) {
    printf("[ERR Data 1] wrong data received\n");
    *Error = MC_ERR_RUNTIME;
  }

  char num_text[4] = {0};
  sprintf(num_text, "%03u", TestCounter % 1000);
  if (0 != memcmp(num_text, &buffer.data[2], 3)) {
    printf("[ERR Data 1] wrong data received\n");
    *Error = MC_ERR_RUNTIME;
  }

  IsStringReceived = true;
}

static void on_large_received(mc_msg_id id, mc_buffer buffer)
{
  if ((101 != id) || mc_buffer_is_null(buffer) || (32 * sizeof(uint32_t) != mc_buffer_get_size(buffer))) {
    *Error = MC_ERR_RUNTIME;
    return;
  }

  const uint32_t* const data = (const uint32_t* const)buffer.data;
  const uint32_t count = mc_buffer_get_size(buffer) / sizeof(*data);

  for (uint32_t index = 0; index < count; index++) {
    const uint32_t expected = ((index & 1) ? -56374141.31 : +8644397.79) * (index + 1) * (TestCounter + 1) + index;
    if (data[index] != expected) {
      printf("[ERR Data 2] Received: %u, Expected: %u\n", data[index], expected);
      *Error = MC_ERR_RUNTIME;
    }
  }

  IsLargeReceived = true;
}

static void on_tiny_received(mc_msg_id id, mc_buffer buffer)
{
  if ((19 != id) || mc_buffer_is_null(buffer) || (sizeof(bool) != mc_buffer_get_size(buffer))) {
    *Error = MC_ERR_RUNTIME;
    return;
  }

  const bool* const data = (const bool* const)buffer.data;

  if ((TestCounter & 1) != *data) {
    printf("[ERR Data 1] wrong data received\n");
    *Error = MC_ERR_RUNTIME;
  }

  IsTinyReceived = true;
}

static void on_signal_received(mc_msg_id id, mc_buffer buffer)
{
  if ((9910 != id) || !mc_buffer_is_empty(buffer)) {
    *Error = MC_ERR_RUNTIME;
    return;
  }

  IsSignalReceived = true;
}

static bool init(void* data)
{
  /* TODO(MN): invalid header, incomplete packet, miss packet pointer, use zero copy
   */
  Error = (mc_error*)data;
  *Error = MC_SUCCESS;

  server_create();
  flush_receive_buffer();

  const mc_msg_cfg config =
  {
    .io = mc_io(server_read, server_write),
    .window_size = 37,
    .recv_pool_size = 150,
    .ids_capacity = 10
  };
  const mc_result_u32 result_u32 = mc_msg_get_alloc_size(config);
  if (MC_SUCCESS != result_u32.error) {
    *Error = result_u32.error;
    return false;
  }

  const mc_result_ptr result = mc_msg_init(mc_buffer(AllocBuffer, sizeof(AllocBuffer)), config);
  if (MC_SUCCESS != result.error) {
    *Error = result.error;
    return false;
  }
  message = result.data;

  mc_error error = MC_SUCCESS;
  error = mc_msg_subscribe(message, 77, on_string_received);
  if (MC_SUCCESS != error) {
    *Error = error;
    return false;
  }
  error = mc_msg_subscribe(message, 101, on_large_received);
  if (MC_SUCCESS != error) {
    *Error = error;
    return false;
  }
  error = mc_msg_subscribe(message, 19, on_tiny_received);
  if (MC_SUCCESS != error) {
    *Error = error;
    return false;
  }

  error = mc_msg_subscribe(message, 9910, on_signal_received);
  if (MC_SUCCESS != error) {
    *Error = error;
    return false;
  }

  BeginTime = mc_now();
  return true;
}

static void print_log()
{
  const float duration_s = (EndTime - BeginTime) / 1000000000.0F;
  const uint32_t size_1 = 9 * sizeof(char);
  const uint32_t size_2 = 32 * sizeof(uint32_t);
  const uint32_t size_3 = 1 * sizeof(bool);
  const uint32_t size_4 = 0;
  const uint32_t size_k_byte_ps = (size_1 + size_2 + size_3 + size_4) * cfg_get_iterations() / 1024;
  const float throughput = size_k_byte_ps / duration_s;

  const uint32_t recv_cnt = cfg_get_recv_counter();
  const uint32_t send_cnt = cfg_get_send_counter();
  const uint32_t recv_failed_cnt = cfg_get_recv_failed_counter();
  const uint32_t send_failed_cnt = cfg_get_send_failed_counter();
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
  const mc_time_t end_time = mc_now_u() + 500000 * ((cfg_get_loss_rate() / 10) + 1);

  while (mc_now_u() < end_time) {
    mc_msg_update(message);
  }
}

static bool all_messages_received()
{
  return (IsStringReceived && IsLargeReceived && IsTinyReceived && IsSignalReceived);
}

static void reset_flags()
{
  IsStringReceived = false;
  IsLargeReceived  = false;
  IsTinyReceived   = false;
  IsSignalReceived = false;
}

static bool recv_messages()
{
  do {
    const mc_error error = mc_msg_update(message);
    if ((MC_SUCCESS != error) && (MC_ERR_NO_SPACE != error)) {
      *Error = MC_ERR_TIMEOUT;
      break;
    }
  } while (!all_messages_received() && (MC_SUCCESS == *Error));

  return (MC_SUCCESS == *Error);
}

void* rcv_start(void* data)
{
  if (!init(data)) {
    return NULL;
  }

  for (TestCounter = 0; TestCounter <= cfg_get_iterations(); TestCounter++) {
    if (!recv_messages()) {
      break;
    }
    
    reset_flags();
    print_progress(TestCounter / (float)cfg_get_iterations());
  }

  if (MC_SUCCESS == *Error) {
    const mc_result_bool result = mc_msg_flush(message, TEST_TIMEOUT_US);
    if ((MC_SUCCESS != result.error) || !result.value) {
      printf("mc_comm_flush failed\n");
      *Error = MC_ERR_TIMEOUT;
    }
  }

  wait_for_sender();
  deinit();
  return NULL;
}
