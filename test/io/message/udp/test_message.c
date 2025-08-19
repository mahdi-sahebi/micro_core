#include <stdio.h>
#include <pthread.h>
#include "core/error.h"
#include "core/version.h"
#include "core/time.h"
#include "alg/mc_buffer.h"
#include "io/message/mc_message.h"


static uint32_t io_recv(void* const data, uint32_t size)
{
  return 0;
}

static uint32_t io_send(const void* const data, uint32_t size)
{
  return 0;
}

static int invalid_creation()
{
  mc_result_ptr result;
  char memory[100];
  mc_msg_cfg config = {0};
  
  result = mc_msg_init(mc_buffer(memory, sizeof(memory)), config);
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  // TODO(MN): Must larger than window_size
  config = mc_msg_cfg(mc_io(io_recv, io_send), 100, 0);
  result = mc_msg_init(mc_buffer(memory, sizeof(memory)), config);
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_msg_cfg(mc_io(io_recv, io_send), 0, 37);
  result = mc_msg_init(mc_buffer(memory, sizeof(memory)), config);
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_msg_cfg(mc_io(NULL, io_send), 100, 37);
  result = mc_msg_init(mc_buffer(memory, sizeof(memory)), config);
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_msg_cfg(mc_io(io_recv, NULL), 100, 37);
  result = mc_msg_init(mc_buffer(memory, sizeof(memory)), config);
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_msg_cfg(mc_io(io_recv, io_send), 100, 37);
  result = mc_msg_init(mc_buffer(memory, 0), config);
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_msg_cfg(mc_io(io_recv, io_send), 100, 37);
  result = mc_msg_init(mc_buffer(NULL, sizeof(memory)), config);
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_msg_cfg(mc_io(io_recv, io_send), 2048, 37);
  result = mc_msg_init(mc_buffer(memory, sizeof(memory)), config);
  if (MC_ERR_BAD_ALLOC != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  return MC_ERR_RUNTIME;
}

static int valid_creation()
{
  mc_result_ptr result;
  char memory[2048];

  mc_msg_cfg config = mc_msg_cfg(mc_io(io_recv, io_send), 100, 37);
  result = mc_msg_init(mc_buffer(memory, sizeof(memory)), config);
  if ((MC_SUCCESS != result.error) || (NULL == result.data)) {
    return MC_ERR_BAD_ALLOC;
  }
  
  return MC_ERR_RUNTIME;
}

static int authentication()
{
  return MC_ERR_RUNTIME;
}

static int singly_high_lossy()
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
  mc_error result = MC_SUCCESS;

  printf("[invalid_creation]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    result = invalid_creation();
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[valid_creation]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    result = valid_creation();
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[invalid_creation]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    result = invalid_creation();
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  return MC_SUCCESS;
}
