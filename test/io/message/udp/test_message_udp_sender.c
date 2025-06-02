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
static uint32_t LastTickUS = 0;
static uint32_t* Result = NULL;


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

static void update_data(uint32_t* const buffer, uint32_t packet_id)
{
  for (uint32_t index = 0; index < DATA_LEN; index++) {
      buffer[index] = (packet_id * DATA_LEN) + index;
  }
}

void* snd_start(void* data)
{
  uint32_t buffer[DATA_LEN];
  uint32_t counter = 0;

  Result = (uint32_t*)data;
  *Result = MC_SUCCESS;

  client_create();
  mc_msg_t* message = mc_msg_new(client_read, client_write, DATA_LEN * sizeof(uint32_t), 3, NULL);

  update_data(buffer, counter);
  counter++;

  /* Let receiver not to miss any packet */
  usleep(200000);
  LastTickUS = TimeNowU();

  while (counter < COMPLETE_COUNT) {
    if (sizeof(buffer) != mc_msg_write(message, buffer, sizeof(buffer))) {
      if ((TimeNowU() - LastTickUS) > TEST_TIMEOUT) {
        *Result = MC_ERR_TIMEOUT;
        break;
      }

      continue;
    }

    update_data(buffer, counter);
    counter++;
    LastTickUS = TimeNowU();
    usleep(10000);
  }

  mc_msg_write_finish(message, 0);
  mc_msg_free(&message);
  client_close();
  return NULL;
}

