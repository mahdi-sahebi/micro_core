#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "core/error.h"
#include "io/message/message.h"
#include "test_message_udp_common.h"
#include "test_message_udp_sender.h"


static int ClientSocket = -1;
static mc_msg_t* message = NULL;
static uint32_t SendCounter = 0;
static uint32_t LastTickUS = 0;
static uint32_t* Result = NULL;
static uint32_t Buffer[DATA_LEN];



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

static void update_data(uint32_t* const Buffer)
{
  for (uint32_t index = 0; index < DATA_LEN; index++) {
    Buffer[index] = (SendCounter * DATA_LEN) + index;
  }

  SendCounter++;
  LastTickUS = TimeNowU();
  usleep(100);
}

static void init(void* data)
{
  Result = (uint32_t*)data;
  *Result = MC_SUCCESS;
  
  SendCounter = 0;
  client_create();
  let_server_start();

  message = mc_msg_new(client_read, client_write, 16 + DATA_LEN * sizeof(uint32_t), 3, NULL, TimeNowU);
}

static void deinit()
{
  mc_msg_free(&message);
  client_close();
}

static bool timed_out()
{
  return ((TimeNowU() - LastTickUS) > TEST_TIMEOUT);
}

void* snd_start(void* data)
{
  init(data);
  update_data(Buffer);

  while (SendCounter <= cfg_get_iterations()) {
    if (timed_out()) {
      *Result = MC_ERR_TIMEOUT;
      break;
    }
    
    if (sizeof(Buffer) != mc_msg_send(message, Buffer, sizeof(Buffer))) {
      continue;
    }

    update_data(Buffer);
  }
  
  if ((MC_SUCCESS == *Result) && !mc_msg_flush(message, TEST_TIMEOUT)) {
    printf("mc_msg_flush failed\n");
    *Result = MC_ERR_TIMEOUT;
  }

  deinit();
  return NULL;
}

