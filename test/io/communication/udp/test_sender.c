#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "core/error.h"
#include "core/time.h"
#include "io/communication/communication.h"
#include "test_common.h"
#include "test_sender.h"


static int ClientSocket = -1;
static mc_comm* message = NULL;
static uint32_t* Result = NULL;
static char TempBuffer[6 * 1024] = {0};
static mc_buffer AllocBuffer = mc_buffer(TempBuffer, sizeof(TempBuffer));


static void client_create()
{
  ClientSocket = socket(AF_INET, SOCK_DGRAM, 0);
  
  struct sockaddr_in addr_in;
  memset(&addr_in, 0, sizeof(addr_in));
  addr_in.sin_family = AF_INET;
  addr_in.sin_port = htons(CLIENT_PORT);
  inet_aton("127.0.0.1", &addr_in.sin_addr);
  bind(ClientSocket, (struct sockaddr*)&addr_in, sizeof(addr_in));

  struct timeval timeout = {0, 10000};
  setsockopt(ClientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

static uint32_t client_write(cvoid* const data, uint32_t size)
{
  return socket_write(ClientSocket, data, size, "127.0.0.1", SERVER_PORT);
}

static uint32_t client_read(void* data, uint32_t size)
{
  return socket_read(ClientSocket, data, size);
}

static void client_close()
{
  close(ClientSocket);
}

static void let_server_start()
{
  /* Let receiver not to miss any packet */
  usleep(200000);
}

static bool init(void* data)
{
  Result = (uint32_t*)data;
  *Result = MC_SUCCESS;
  
  client_create();
  let_server_start();
  memset(TempBuffer, 0x00, sizeof(TempBuffer));

  const mc_comm_cfg config = mc_comm_cfg(mc_io(client_read, client_write), 
    mc_comm_wnd(59, 2), mc_comm_wnd(1157, 4));
    
  const mc_u32 result_u32 = mc_comm_get_alloc_size(config);
  if (MC_SUCCESS != result_u32.error) {
    *Result = result_u32.error;
    return false;
  }
  if (AllocBuffer.capacity < result_u32.value) {
    printf("[Send] Not enough space for test\n");
    *Result = MC_ERR_OUT_OF_RANGE;
    return false;
  }
  
  AllocBuffer.capacity = result_u32.value;
  const mc_ptr result = mc_comm_init(AllocBuffer, config);
  if (MC_SUCCESS != result.error) {
    *Result = result.error;
    return false;
  }

  message = result.data;;
  return true;
}

static void deinit()
{
  client_close();
}

static bool send_data(cvoid* data, uint32_t size)
{
  const mc_u32 result = mc_comm_send(message, data, size, TEST_TIMEOUT_US);
  if ((MC_SUCCESS != result.error) || (result.value != size)) {
    *Result = MC_ERR_TIMEOUT;
    return false;
  }
  return true;
}

static bool send_string(uint32_t seed)
{
  char data[9] = {0};
  const uint32_t size = sizeof(data);
  sprintf(data, "!p%03u.?I", seed % 1000);

  return send_data(data, size);
}

static bool send_variadic_size(uint32_t seed)
{
  uint32_t data[1024] = {0};
  const uint32_t random_count = (seed * 1664525) + 1013904223;
  const uint32_t count = (random_count % 997) + 27;
  const uint32_t size = count * sizeof(*data);

  for (uint32_t index = 0; index < count; index++) {
    data[index] = ((index & 1) ? -56374141.31 : +8644397.79) * (index + 1) * (seed + 1) + index;
  }

  return send_data(data, size);
}

static bool send_tiny_size(uint32_t seed)
{
  bool data = (seed & 1);
  const uint32_t size = sizeof(data);

  return send_data(&data, size);
}

void* snd_start(void* data)
{
  if (!init(data)) {
    return NULL;
  }
  
  for (uint32_t counter = 0; counter <= cfg_get_iterations(); counter++) {
    if (MC_SUCCESS != mc_comm_update(message)) {
      *Result = MC_ERR_TIMEOUT;
      break;
    }

    if (!send_string(counter)        ||
        !send_variadic_size(counter) ||  /* Smaller and larger than window size */
        !send_tiny_size(counter)){
      *Result = MC_ERR_TIMEOUT;
      break;
    }
  }
  
  if (MC_SUCCESS == *Result) {
    const mc_bool result = mc_comm_flush(message, TEST_TIMEOUT_US);
    if ((MC_SUCCESS != result.error) || !result.value) {
      printf("mc_comm_flush failed\n");
      *Result = MC_ERR_TIMEOUT;
    }
  }

  deinit();
  return NULL;
}

