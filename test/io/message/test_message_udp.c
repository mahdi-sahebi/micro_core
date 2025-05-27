/* For testing this module, we need a non-session based communication
 * like UDP, so we just implement a simple send and receive socket.
 */
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "core/error.h"
#include "core/version.h"


#define SERVER_PORT 9000
#define CLIENT_PORT 9001


int socket_create_server()
{
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(CLIENT_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));

    struct timeval timeout = {0, 100000};
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    return socket_fd;
}

int socket_create_client()
{
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    
    return socket_fd;
}

uint32_t socket_write(int socket_fd, const void* data, uint32_t size)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    uint32_t sent_size = sendto(socket_fd, data, size, 0, (struct sockaddr*)&addr, addr_len);

    if (-1 == sent_size) {
      sent_size = 0;
    }

    return sent_size;
}

uint32_t socket_read(int socket_fd, void* data, uint32_t size)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    uint32_t read_size = recvfrom(socket_fd, data, size, 0, (struct sockaddr*)&addr, &addr_len);

    if (-1 == read_size) {
        read_size = 0;
    }
    return read_size;
}

void socket_close(int socket_fd)
{
  close(socket_fd);
}



static void* sender(void* data)
{

}

static void* receiver(void* data)
{

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
