/* For testing this module, we need a non-session based communication
 * like UDP, so we just implement a simple send and receive socket.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "core/error.h"
#include "core/version.h"


#define SERVER_PORT 9000
#define CLIENT_PORT 9001

int ServerSocket = -1;
int ClientSocket = -1;


uint32_t socket_write(int socket_fd, const void* data, uint32_t size, char* const dst_ip, uint16_t dst_port)
{
  struct sockaddr_in addr_in;
  memset(&addr_in, 0, sizeof(addr_in));
  addr_in.sin_family = AF_INET;
  addr_in.sin_port = htons(dst_port);
  addr_in.sin_addr.s_addr = inet_addr(dst_ip);

  socklen_t addr_len = sizeof(addr_in);
  uint32_t sent_size = sendto(socket_fd, data, size, 0, (struct sockaddr*)&addr_in, addr_len);

  if (-1 == sent_size) {
    sent_size = 0;
  }

  return sent_size;
}

uint32_t socket_read(int socket_fd, void* data, uint32_t size)
{
  struct sockaddr_in addr_in;
  socklen_t addr_len = sizeof(addr_in);
  uint32_t read_size = recvfrom(socket_fd, data, size, 0, (struct sockaddr*)&addr_in, &addr_len);

  char sender_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &addr_in.sin_addr, sender_ip, INET_ADDRSTRLEN);
  const uint16_t sender_port = ntohs(addr_in.sin_port);

  if (-1 == read_size) {
      read_size = 0;
  }
  return read_size;
}

void server_create()
{
  ServerSocket = socket(AF_INET, SOCK_DGRAM, 0);
  
  struct sockaddr_in addr_in;
  memset(&addr_in, 0, sizeof(addr_in));
  addr_in.sin_family = AF_INET;
  addr_in.sin_port = htons(SERVER_PORT);
  addr_in.sin_addr.s_addr = INADDR_ANY;
  bind(ServerSocket, (struct sockaddr*)&addr_in, sizeof(addr_in));

  struct timeval timeout = {0, 100000};
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

void client_create()
{
    ClientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in addr_in;
    memset(&addr_in, 0, sizeof(addr_in));
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(CLIENT_PORT);
    inet_aton("127.0.0.1", &addr_in.sin_addr);

    struct timeval timeout = {0, 100000};
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



static void* sender(void* data)
{
  return NULL;
}

static void* receiver(void* data)
{
  return NULL;
}

static int full()
{
  pthread_t task_sender;
  pthread_t task_receiver;

  if (pthread_create(&task_sender, NULL, sender, NULL) || 
      pthread_create(&task_receiver, NULL, receiver, NULL)) {
    MC_ERR_BAD_ALLOC;
  }

  if (pthread_join(task_sender, NULL) || 
      pthread_join(task_receiver, NULL)) {
    return MC_ERR_RUNTIME;
  }

  return MC_SUCCESS;
}

int main()
{
  printf("[MICRO CORE %u.%u.%u - IO - MESSAGE]\n", MC_VERSION_MAJOR, MC_VERSION_MINOR, MC_VERSION_PATCH);
  mc_result result = MC_SUCCESS;

  printf("[full]\n");
  result = full();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: %u\n", result);
    return result;
  }
  printf("[PASSED]\n");

  printf("passed\n");
  return MC_SUCCESS;
}
