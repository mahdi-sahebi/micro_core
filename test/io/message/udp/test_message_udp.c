/* For testing this module, we need a non-session based communication
 * like UDP, so we just implement a simple send and receive socket.
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "core/error.h"
#include "core/version.h"
#include "test_message_udp_common.h"
#include "test_message_udp_receiver.h"
#include "test_message_udp_sender.h"


static int hulf_duplex()
{
  pthread_t task_sender;
  pthread_t task_receiver;

  if (pthread_create(&task_sender,   NULL, snd_start, NULL) || 
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
