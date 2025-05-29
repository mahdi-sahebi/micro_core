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
  pthread_t task_snd;
  pthread_t task_rcv;
  uint32_t snd_error = MC_SUCCESS;
  uint32_t rcv_error = MC_SUCCESS;

  if (pthread_create(&task_snd, NULL, snd_start, &snd_error) || 
      pthread_create(&task_rcv, NULL, rcv_start, &rcv_error)) {
    MC_ERR_BAD_ALLOC;
  }


  if (pthread_join(task_snd, NULL) || 
      pthread_join(task_rcv, NULL)) {
    return MC_ERR_RUNTIME;
  }

  if (snd_error) {
    return snd_error;
  }
  if (rcv_error) {
    return rcv_error;
  }
  
  return MC_SUCCESS;
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
