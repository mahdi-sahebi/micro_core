
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "core/error.h"
#include "core/version.h"
#include "test_message_udp_common.h"
#include "test_message_udp_receiver.h"


static int ServerSocket = -1;
static uint32_t ReceiveCounter = 0;

void server_create()
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

uint32_t server_write(const void* data, uint32_t size)
{
  return socket_write(ServerSocket, data, size, "127.0.0.1", CLIENT_PORT);
}

uint32_t server_read(void* data, uint32_t size)
{
  return socket_read(ServerSocket, data, size);
}

void server_close()
{
  close(ServerSocket);
}

static void on_receive(void* const data, uint32_t size)
{
  if ((DATA_LEN * sizeof(uint32_t)) != size) {
    Error = true;
    return;
  }

  const uint32_t* const buffer = (uint32_t*)data;

  if (!verify_data(buffer, ReceiveCounter)) {
    Error = true;
    return;
  }

  ReceiveCounter++;
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

void* rcv_start(void* data)
{
  /* TODO(MN): Repetitive packet, invalid header, incomplete packet, miss packet pointer, use zero copy
   */
  ReceiveCounter = 0;
  Error = false;
  server_create();
  mc_msg_t* const message = mc_msg_new(client_write, client_read, sizeof(buffer), 3, on_receive);

  while (ReceiveCounter < (COMPLETE_COUNT - 1)) {
    mc_msg_read(message);
  }

  mc_msg_read_finish(message);
  mc_msg_free(message);
  server_close();
  return NULL;
}
