#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "core/error.h"
#include "core/time.h"
#include "io/communication/communication.h"
#include "test_communication_udp_common.h"
#include "test_communication_udp_sender.h"


static int ClientSocket = -1;
static mc_comm_t* message = NULL;
static mc_time_t LastTickUS = 0;
static uint32_t* Result = NULL;
static mc_span AllocBuffer = {0};


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

static uint32_t client_write(const void* const data, uint32_t size)
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

static void init(void* data)
{
  Result = (uint32_t*)data;
  *Result = MC_SUCCESS;
  
  client_create();
  let_server_start();

  const uint32_t window_size = 37;
  const uint32_t window_capacity = 3;// TODO(MN): Calculate accoridng the buffer size / window size
  const uint32_t alloc_size = mc_comm_get_alloc_size(window_size, window_capacity).value;
  AllocBuffer = mc_span(malloc(alloc_size), alloc_size);

  message = mc_comm_init(AllocBuffer, window_size, window_capacity, mc_io(client_read, client_write));
}

static void deinit()
{
  client_close();
  free(AllocBuffer.data);
}

static bool timed_out()
{
  return ((mc_now_u() - LastTickUS) > TEST_TIMEOUT_US);
}

static bool send_data(const void* data, uint32_t size)
{
  // TODO(MN): Use timed_out as an arg
  if (mc_comm_send(message, data, size) != size) {// TODO(MN): Pass timeout as an arg
    *Result = MC_ERR_TIMEOUT;
    return false;
  }

  LastTickUS = mc_now_u();
  return true;
}

static bool send_data_string(uint32_t seed)
{
  char data[9] = {0};
  const uint32_t size = sizeof(data);
  sprintf(data, "!p%03u.?I", seed % 1000);

  return send_data(data, size);
}

static bool send_data_variadic_size(uint32_t seed)
{
  uint32_t data[30] = {0};
  const uint32_t random_count = (seed * 1664525) + 1013904223;
  const uint32_t count = (random_count % 28) + 2;
  const uint32_t size = count * sizeof(*data);

  for (uint32_t index = 0; index < count; index++) {
    data[index] = ((index & 1) ? -56374141.31 : +8644397.79) * (index + 1) * (seed + 1) + index;
  }

  return send_data(data, size);
}

static bool send_data_tiny_size(uint32_t seed)
{
  bool data = (seed & 1);
  const uint32_t size = sizeof(data);

  return send_data(&data, size);
}

void* snd_start(void* data)
{
  init(data);
  
  for (uint32_t counter = 0; counter <= cfg_get_iterations(); counter++) {
    mc_comm_update(message);

    // if (timed_out()) {
    //   *Result = MC_ERR_TIMEOUT;
    //   break;
    // }

    if (!send_data_string(counter)        ||
        !send_data_variadic_size(counter) ||  /* Smaller and larger than window size */
        !send_data_tiny_size(counter)){
      *Result = MC_ERR_TIMEOUT;
      break;
    }
  }
  
  if ((MC_SUCCESS == *Result) && !mc_comm_flush(message, TEST_TIMEOUT_US)) {
    printf("mc_comm_flush failed\n");
    *Result = MC_ERR_TIMEOUT;
  }

  deinit();
  return NULL;
}

