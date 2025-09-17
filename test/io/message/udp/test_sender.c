#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "core/error.h"
#include "core/time.h"
#include "io/message/mc_message.h"
#include "test_common.h"
#include "test_sender.h"


static int ClientSocket = -1;
static mc_msg* message = NULL;
static mc_error* Error = NULL;
static char AllocBuffer[1 * 1024];


static void client_create()
{
  ClientSocket = socket(AF_INET, SOCK_DGRAM, 0);
  
  struct timeval timeout = {0, 10000};
  setsockopt(ClientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

static uint32_t client_write(const void* const data, uint32_t size)
{
  return socket_write(ClientSocket, data, size, "127.0.0.1", SERVER_PORT);
}

static uint32_t client_read(void* data, uint32_t size)
{
  char src_ip[INET_ADDRSTRLEN] = {0};
  uint16_t src_port = 0;
  const uint32_t read_size = socket_read(ClientSocket, data, size, src_ip, &src_port);

  if ((SERVER_PORT != src_port) || (0 != strcmp(src_ip, "127.0.0.1"))) {
    return 0;
  }

  return read_size;
}

static void client_close()
{
  close(ClientSocket);
}

static void let_server_start()
{
  /* Let receiver not to miss any packet */
  usleep(1000000);// TODO(MN): 200ms?
}

static bool init(void* data)
{
  Error = (mc_error*)data;
  *Error = MC_SUCCESS;
  
  client_create();
  let_server_start();

  const mc_msg_cfg config =
  {
    .io = mc_io(client_read, client_write),
    .recv = mc_comm_wnd(37, 2),
    .send = mc_comm_wnd(37, 2),
    .pool_size = 120,
    .ids_capacity = 0
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

  message = result.data;;
  return true;
}

static void deinit()
{
  client_close();
}

static bool send_data(mc_buffer buffer, mc_msg_id id)
{
  const mc_result_u32 result = mc_msg_send(message, buffer, id, TEST_TIMEOUT_US);
  if ((MC_SUCCESS != result.error) || (result.value != mc_buffer_get_size(buffer))) {
    *Error = MC_ERR_TIMEOUT;
    return false;
  }
  return true;
}

static bool send_string(uint32_t seed)
{
  char data[9] = {0};
  const uint32_t size = sizeof(data);
  sprintf(data, "!p%03u.?I", seed % 1000);

  return send_data(mc_buffer(data, size), 77);
}

static bool send_large_1(uint32_t seed)
{
  uint32_t data[32] = {0};
  const uint32_t count = sizeof(data) / sizeof(*data);

  for (uint32_t index = 0; index < count; index++) {
    data[index] = ((index & 1) ? -56374141.31 : +8644397.79) * (index + 1) * (seed + 1) + index;
  }

  return send_data(mc_buffer(data, sizeof(data)), 101);
}

static bool send_large_2(uint32_t seed)
{
  char data[189] = {0};
  return send_data(mc_buffer(data, sizeof(data)), 436);
}

static bool send_tiny(uint32_t seed)
{
  bool data = (seed & 1);
  const uint32_t size = sizeof(data);

  return send_data(mc_buffer(&data, size), 19);
}

static bool send_signal()
{
  return (MC_SUCCESS == mc_msg_signal(message, 9910, TEST_TIMEOUT_US).error);
}

static bool send_messages(uint32_t seed)
{
  if (MC_SUCCESS != mc_msg_update(message)) {
    return false;
  }

  return (send_string(seed)  &&
          send_large_1(seed) &&
          send_large_2(seed) &&
          send_tiny(seed)    &&
          send_signal());
}

void* snd_start(void* data)
{
  if (!init(data)) {
    return NULL;
  }
  
  for (uint32_t counter = 0; counter <= cfg_get_iterations(); counter++) {
    if (!send_messages(counter)) {
      *Error = MC_ERR_TIMEOUT;
      break;
    }
  }
  
  if (MC_SUCCESS == *Error) {
    const mc_result_bool result = mc_msg_flush(message, TEST_TIMEOUT_US);
    if ((MC_SUCCESS != result.error) || !result.value) {
      printf("mc_comm_flush failed\n");
      *Error = MC_ERR_TIMEOUT;
    }
  }

  deinit();
  return NULL;
}

