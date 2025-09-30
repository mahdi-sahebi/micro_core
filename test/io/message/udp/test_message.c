#include <stdio.h>
#include <pthread.h>
#include "core/error.h"
#include "core/version.h"
#include "core/time.h"
#include "alg/mc_buffer.h"
#include "io/message/mc_message.h"
#include "test_common.h"
#include "test_sender.h"
#include "test_receiver.h"


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

  config = mc_msg_cfg(mc_io(io_recv, io_send), mc_comm_wnd(100, 0), mc_comm_wnd(100, 100), 50, 5);
  result = mc_msg_init(mc_buffer(memory, sizeof(memory)), config);
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_msg_cfg(mc_io(io_recv, io_send), mc_comm_wnd(0, 100), mc_comm_wnd(100, 100), 50, 5);
  result = mc_msg_init(mc_buffer(memory, sizeof(memory)), config);
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_msg_cfg(mc_io(io_recv, io_send), mc_comm_wnd(100, 100), mc_comm_wnd(0, 100), 50, 5);
  result = mc_msg_init(mc_buffer(memory, sizeof(memory)), config);
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_msg_cfg(mc_io(io_recv, io_send), mc_comm_wnd(100, 100), mc_comm_wnd(100, 0), 50, 5);
  result = mc_msg_init(mc_buffer(memory, sizeof(memory)), config);
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_msg_cfg(mc_io(NULL, io_send), mc_comm_wnd(100, 100), mc_comm_wnd(100, 100), 50, 5);
  result = mc_msg_init(mc_buffer(memory, sizeof(memory)), config);
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_msg_cfg(mc_io(io_recv, NULL), mc_comm_wnd(100, 100), mc_comm_wnd(100, 100), 50, 5);
  result = mc_msg_init(mc_buffer(memory, sizeof(memory)), config);
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_msg_cfg(mc_io(io_recv, io_send), mc_comm_wnd(100, 100), mc_comm_wnd(100, 100), 50, 5);
  result = mc_msg_init(mc_buffer(memory, 0), config);
  if (MC_ERR_BAD_ALLOC != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_msg_cfg(mc_io(io_recv, io_send), mc_comm_wnd(100, 100), mc_comm_wnd(100, 100), 50, 5);
  result = mc_msg_init(mc_buffer(NULL, sizeof(memory)), config);
  if (MC_ERR_BAD_ALLOC != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_msg_cfg(mc_io(io_recv, io_send), mc_comm_wnd(100, 100), mc_comm_wnd(100, 100), 50, 5);
  result = mc_msg_init(mc_buffer(memory, sizeof(memory)), config);
  if (MC_ERR_BAD_ALLOC != result.error) {
    return MC_ERR_BAD_ALLOC;
  }

  return MC_SUCCESS;
}

static int valid_creation()
{
  mc_result_ptr result;
  char memory[2048];

  mc_msg_cfg config = mc_msg_cfg(mc_io(io_recv, io_send), mc_comm_wnd(100, 3), mc_comm_wnd(50, 2), 200, 15);
  result = mc_msg_init(mc_buffer(memory, sizeof(memory)), config);
  if ((MC_SUCCESS != result.error) || (NULL == result.data)) {
    return MC_ERR_BAD_ALLOC;
  }
  
  return MC_SUCCESS;
}

static int singly_direction()
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

static int singly_repetitive()
{
  cfg_set_repetitive_send(true);
  cfg_set_iterations(100);
  const int result = singly_direction();
  cfg_set_repetitive_send(true);
  return result;
}

static int singly_low_lossy()
{
  cfg_set_loss_rate(20);
  cfg_set_iterations(100);
  const int result = singly_direction();
  cfg_set_loss_rate(0);
  return result;
}

static int singly_high_lossy()
{
  cfg_set_loss_rate(98);
  cfg_set_iterations(50);
  cfg_set_periodic_duration(5000);
  const int result = singly_direction();
  cfg_set_loss_rate(0);
  return result;
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

  printf("[singly_direction]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    result = singly_direction();
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[singly_repetitive]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    result = singly_repetitive();
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[singly_low_lossy]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    result = singly_low_lossy();
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[singly_high_lossy]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    result = singly_high_lossy();
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  return MC_SUCCESS;
}
