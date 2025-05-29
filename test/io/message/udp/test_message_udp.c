/* For testing this module, we need a non-session based communication
 * like UDP, so we just implement a simple send and receive socket.
 * creation, invalid parameters, ...
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "core/error.h"
#include "core/version.h"
#include "io/message/message.h"
#include "test_message_udp_common.h"
#include "test_message_udp_receiver.h"
#include "test_message_udp_sender.h"


static uint32_t read_api(void* const data, uint32_t size)
{
    return 0;
}

static uint32_t write_api(const void* const data, uint32_t size)
{
    return 0;
}

static int invalid_creation()
{
  mc_msg_t* message = NULL;
  
  message = mc_msg_new(NULL, write_api, DATA_LEN * sizeof(uint32_t), 3, NULL);
  if (NULL != message) {
    return MC_ERR_BAD_ALLOC;
  }

  message = mc_msg_new(read_api, NULL, DATA_LEN * sizeof(uint32_t), 3, NULL);
  if (NULL != message) {
    return MC_ERR_BAD_ALLOC;
  }

  message = mc_msg_new(read_api, write_api, 0, 3, NULL);
  if (NULL != message) {
    return MC_ERR_BAD_ALLOC;
  }

  message = mc_msg_new(read_api, write_api, 0, 0, NULL);
  if (NULL != message) {
    return MC_ERR_BAD_ALLOC;
  }

  return MC_SUCCESS;
}

static int valid_creation()
{
  mc_msg_t* message = NULL;
  
  message = mc_msg_new(read_api, write_api, 5 * sizeof(uint32_t), 3, NULL);
  if (NULL == message) {
    return MC_ERR_BAD_ALLOC;
  }

  mc_msg_free(&message);
  if (NULL != message) {
    return MC_ERR_BAD_ALLOC;
  }

  return MC_SUCCESS;
}

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

  printf("[invalid_creation]\n");
  result = invalid_creation();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: %u\n", result);
    return result;
  }
  printf("[PASSED]\n");

  printf("[valid_creation]\n");
  result = valid_creation();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: %u\n", result);
    return result;
  }
  printf("[PASSED]\n");

  printf("[hulf_duplex]\n");
  result = hulf_duplex();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: %u\n", result);
    return result;
  }
  printf("[PASSED]\n");

  return MC_SUCCESS;
}
