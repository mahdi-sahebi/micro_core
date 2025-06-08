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

  message = mc_msg_new(read_api, write_api, 1, 3, NULL);
  if (NULL != message) {
    return MC_ERR_BAD_ALLOC;
  }

  return MC_SUCCESS;
}

static int valid_creation()
{
  const uint32_t capcity = 3;
  mc_msg_t* message = NULL;
  
  message = mc_msg_new(read_api, write_api, 5 * sizeof(uint32_t), capcity, NULL);
  if (NULL == message) {
    return MC_ERR_BAD_ALLOC;
  }

  if (capcity != mc_msg_get_capacity(message)) {
    return MC_ERR_BAD_ALLOC;
  }

  mc_msg_free(&message);
  if (NULL != message) {
    return MC_ERR_BAD_ALLOC;
  }

  return MC_SUCCESS;
}

static int get_status()
{
  uint32_t data[5];
  mc_msg_t* message = mc_msg_new(read_api, write_api, 36, 3, NULL);
  if (NULL == message) {
    return MC_ERR_BAD_ALLOC;
  }

  /* Empty state */
  if (!mc_msg_is_empty(message)) {
    return MC_ERR_RUNTIME;
  }
  if (mc_msg_is_full(message)) {
    return MC_ERR_RUNTIME;
  }

  /* Adding */
  const uint32_t capacity = mc_msg_get_capacity(message);
  for (uint32_t index = 0; index < capacity; index++) {
    if (mc_msg_write(message, data, sizeof(data)) != sizeof(data)) {
      return MC_ERR_INCOMPLETE;
    }

    if (mc_msg_get_count(message) != (index + 1)) {
      return MC_ERR_MEMORY_OUT_OF_RANGE;
    }
  }

  /* Full state */
  if (mc_msg_is_empty(message)) {
    return MC_ERR_RUNTIME;
  }
  if (!mc_msg_is_full(message)) {
    return MC_ERR_RUNTIME;
  }


  mc_msg_free(&message);
  if (NULL != message) {
    return MC_ERR_BAD_ALLOC;
  }

  return MC_SUCCESS;
}

static int clear()
{
  uint32_t data[5];
  mc_msg_t* message = mc_msg_new(read_api, write_api, 36, 3, NULL);

  mc_result result = mc_msg_clear(NULL);
  if (MC_ERR_INVALID_ARGUMENT != result) {
    return result;
  }

  result = mc_msg_clear(message);
  if (MC_SUCCESS != result) {
    return result;
  }

  if (!mc_msg_is_empty(message)) {
    return MC_ERR_RUNTIME;
  }

  for (uint32_t index = 0; index < mc_msg_get_capacity(message); index++) {
    mc_msg_write(message, data, sizeof(data)) != sizeof(data);
  }

  if (mc_msg_is_empty(message) || !mc_msg_is_full(message)) {
    return MC_ERR_RUNTIME;
  }

  result = mc_msg_clear(message);
  if (MC_SUCCESS != result) {
    return result;
  }

  if (!mc_msg_is_empty(message) || mc_msg_is_full(message)) {
    return MC_ERR_RUNTIME;
  }

  mc_msg_free(&message);
  if (NULL != message) {
    return MC_ERR_BAD_ALLOC;
  }

  return MC_SUCCESS;
}

static int singly_direction()
{
  cfg_set_repetitive_send(false);

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

static int singly_repetitive()
{
  cfg_set_repetitive_send(true);
  const int result = singly_direction();
  cfg_set_repetitive_send(true);
  return result;
}

static int singly_low_lossy()
{
  cfg_set_loss_rate(10);
  const int result = singly_direction();
  cfg_set_loss_rate(0);
  return result;
}

static int singly_high_lossy()
{
  cfg_set_loss_rate(70);
  const int result = singly_direction();
  cfg_set_loss_rate(0);
  return result;
}

static int small_write()
{
  return MC_ERR_RUNTIME;
}

static int large_write()
{
  return MC_ERR_RUNTIME;
}

static int hulf_duplex()
{
  return MC_ERR_RUNTIME;
}

static int full_duplex()
{
  return MC_ERR_RUNTIME;
}

int main()
{
  printf("[MICRO CORE %u.%u.%u - IO - MESSAGE]\n", MC_VERSION_MAJOR, MC_VERSION_MINOR, MC_VERSION_PATCH);
  mc_result result = MC_SUCCESS;

  printf("[invalid_creation]\n");
  result = invalid_creation();
  if (MC_SUCCESS != result) {
    printf("FAILED: %u\n\n", result);
  } else {
    printf("PASSED\n\n");
  }


  printf("[valid_creation]\n");
  result = valid_creation();
  if (MC_SUCCESS != result) {
    printf("FAILED: %u\n\n", result);
  } else {
    printf("PASSED\n\n");
  }


  printf("[get_status]\n");
  result = get_status();
  if (MC_SUCCESS != result) {
    printf("FAILED: %u\n\n", result);
  } else {
    printf("PASSED\n\n");
  }


  printf("[clear]\n");
  result = clear();
  if (MC_SUCCESS != result) {
    printf("FAILED: %u\n\n", result);
  } else {
    printf("PASSED\n\n");
  }


  printf("[singly_direction]\n");
  result = singly_direction();
  if (MC_SUCCESS != result) {
    printf("FAILED: %u\n\n", result);
  } else {
    printf("PASSED\n\n");
  }


  printf("[singly_repetitive]\n");
  result = singly_repetitive();
  if (MC_SUCCESS != result) {
    printf("FAILED: %u\n\n", result);
  } else {
    printf("PASSED\n\n");
  }

  
  printf("[singly_low_lossy]\n");
  result = singly_low_lossy();
  if (MC_SUCCESS != result) {
    printf("FAILED: %u\n\n", result);
  } else {
    printf("PASSED\n\n");
  }

  
  printf("[singly_high_lossy]\n");
  result = singly_high_lossy();
  if (MC_SUCCESS != result) {
    printf("FAILED: %u\n\n", result);
  } else {
    printf("PASSED\n\n");
  }


  printf("[small_write]\n");
  result = small_write();
  if (MC_SUCCESS != result) {
    printf("FAILED: %u\n\n", result);
  } else {
    printf("PASSED\n\n");
  }


  printf("[large_write]\n");
  result = large_write();
  if (MC_SUCCESS != result) {
    printf("FAILED: %u\n\n", result);
  } else {
    printf("PASSED\n\n");
  }


  printf("[hulf_duplex]\n");
  result = hulf_duplex();
  if (MC_SUCCESS != result) {
    printf("FAILED: %u\n\n", result);
  } else {
    printf("PASSED\n\n");
  }


  printf("[full_duplex]\n");
  result = full_duplex();
  if (MC_SUCCESS != result) {
    printf("FAILED: %u\n\n", result);
  } else {
    printf("PASSED\n\n");
  }

  return MC_SUCCESS;
}
