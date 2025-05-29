/* For testing this module, we need a non-session based communication
 * like UDP, so we just implement a simple send and receive socket.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "core/error.h"
#include "core/version.h"
#include "test_message_udp_common.h"



static int ClientSocket = -1;



void client_create()
{
    ClientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in addr_in;
    memset(&addr_in, 0, sizeof(addr_in));
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(CLIENT_PORT);
    inet_aton("127.0.0.1", &addr_in.sin_addr);

    struct timeval timeout = {0, 10000};
    setsockopt(ClientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

uint32_t client_write(const void* data, uint32_t size)
{
    return socket_write(ClientSocket, data, size, "127.0.0.1", SERVER_PORT);
}

uint32_t client_read(void* data, uint32_t size)
{
    return socket_read(ClientSocket, data, size);
}

void client_close()
{
  close(ClientSocket);
}

static void update_data(uint32_t* const buffer, uint32_t packet_id)
{
  for (uint32_t index = 0; index < DATA_LEN; index++) {
      buffer[index] = (packet_id * DATA_LEN) + index;
  }
}

static void* sender(void* data)
{
  uint32_t buffer[DATA_LEN];
  uint32_t counter = 0;
  client_create();
  mc_msg_t* const message = mc_msg_new(server_write, server_read, sizeof(buffer), 3, NULL);
  update_data(buffer, counter);
  counter++;

  /* Let receiver not to miss any packet */
  usleep(200000);

  while (counter < COMPLETE_COUNT) {
    if (sizeof(buffer) != mc_msg_write(message, buffer, sizeof(buffer))) {
      continue;
    }

    update_data(buffer, counter);
    counter++;
    usleep(10000);
  }

  mc_msg_write_finish(message);
  mc_msg_free(message);
  client_close();
  return NULL;
}

static int hulf_duplex()
{
  pthread_t task_sender;
  pthread_t task_receiver;

  if (pthread_create(&task_sender,   NULL, sender,    NULL) || 
      pthread_create(&task_receiver, NULL, rcv_start, NULL)) {
    MC_ERR_BAD_ALLOC;
  }

  if (pthread_join(task_sender,   NULL) || 
      pthread_join(task_receiver, NULL)) {
    return MC_ERR_RUNTIME;
  }

  return Error ? MC_ERR_RUNTIME : MC_SUCCESS;
}

int main()
{
  printf("[MICRO CORE %u.%u.%u - IO - MESSAGE]\n", MC_VERSION_MAJOR, MC_VERSION_MINOR, MC_VERSION_PATCH);
  mc_result result = MC_SUCCESS;

  printf("[hulf_duplex]\n");
  result = hulf_duplex();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: %u\n", result);
    return result;
  }
  printf("[PASSED]\n");

  return MC_SUCCESS;
}
