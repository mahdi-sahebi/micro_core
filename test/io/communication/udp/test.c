/* For testing this module, we need a non-session based communication
 * like UDP, so we just implement a simple send and receive socket.
 * creation, invalid parameters, diff snd/rcv window sizes, ...
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "core/error.h"
#include "core/version.h"
#include "core/time.h"
#include "io/communication/communication.h"
#include "test_common.h"
#include "test_receiver.h"
#include "test_sender.h"


static uint32_t io_recv(void* const data, uint32_t size)
{
  return 0;
}

static uint32_t io_send(cvoid* const data, uint32_t size)
{
  return 0;
}

static int invalid_creation()
{
  char memory[1024];
  mc_buffer alloc_buffer = mc_buffer(memory, sizeof(memory));
  mc_ptr result_ptr = {0};
  mc_comm* message = NULL;
  mc_comm_cfg config = {0};
  
  config = mc_comm_cfg(mc_io(NULL, io_send), mc_comm_wnd(1024, 3), mc_comm_wnd(1024, 3));
  result_ptr = mc_comm_init(alloc_buffer, config);
  if ((MC_SUCCESS == result_ptr.error) || (NULL != result_ptr.data)) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_comm_cfg(mc_io(io_recv, NULL), mc_comm_wnd(1024, 3), mc_comm_wnd(1024, 3));
  result_ptr = mc_comm_init(alloc_buffer, config);
  if ((MC_SUCCESS == result_ptr.error) || (NULL != result_ptr.data)) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_comm_cfg(mc_io(io_recv, io_send), mc_comm_wnd(1024, 3), mc_comm_wnd(0, 3));
  result_ptr = mc_comm_init(alloc_buffer, config);
  if ((MC_SUCCESS == result_ptr.error) || (NULL != result_ptr.data)) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_comm_cfg(mc_io(io_recv, io_send), mc_comm_wnd(0, 3), mc_comm_wnd(1024, 3));
  result_ptr = mc_comm_init(alloc_buffer, config);
  if ((MC_SUCCESS == result_ptr.error) || (NULL != result_ptr.data)) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_comm_cfg(mc_io(io_recv, io_send), mc_comm_wnd(1024, 3), mc_comm_wnd(1024, 3));
  result_ptr = mc_comm_init(alloc_buffer, config);
  if ((MC_SUCCESS == result_ptr.error) || (NULL != result_ptr.data)) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_comm_cfg(mc_io(io_recv, io_send), mc_comm_wnd(1, 3), mc_comm_wnd(1, 3));
  result_ptr = mc_comm_init(alloc_buffer, config);
  if ((MC_SUCCESS == result_ptr.error) || (NULL != result_ptr.data)) {
    return MC_ERR_BAD_ALLOC;
  }

  return MC_SUCCESS;
}

static int valid_creation()
{
  char memory[1024];
  mc_buffer alloc_buffer = mc_buffer(memory, sizeof(memory));
  cuint32_t capcity = 3;
  
  mc_comm_cfg config = mc_comm_cfg(
    mc_io(io_recv, io_send), 
    mc_comm_wnd(5 * sizeof(uint32_t), capcity), 
    mc_comm_wnd(5 * sizeof(uint32_t), capcity));
  const mc_ptr result = mc_comm_init(alloc_buffer, config);
  if ((MC_SUCCESS != result.error) || (NULL == result.data)) {
    return MC_ERR_BAD_ALLOC;
  }

  return MC_SUCCESS;
}

static int singly_direction()
{
  cfg_set_repetitive_send(false);// TODO(MN): Set out of this function

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
  cfg_set_iterations(200);
  cfg_set_periodic_duration(0);
  const int result = singly_direction();
  cfg_set_repetitive_send(true);
  return result;
}

static int singly_low_lossy()
{
  cfg_set_loss_rate(20);
  cfg_set_iterations(200);
  cfg_set_periodic_duration(100);
  const int result = singly_direction();
  cfg_set_loss_rate(0);
  return result;
}

static int singly_high_lossy()
{
  cfg_set_loss_rate(98);
  cfg_set_iterations(100);
  cfg_set_periodic_duration(5000);
  const int result = singly_direction();
  cfg_set_loss_rate(0);
  return result;
}

static int singly_timed_out()
{
  cfg_set_loss_rate(98);
  cfg_set_periodic_duration(5000);
  cfg_set_timeout_us(50);

  char alloc_buffer[200];

  const mc_comm_cfg config = mc_comm_cfg(mc_io(io_recv, io_send), mc_comm_wnd(15, 1), mc_comm_wnd(15, 1));
  mc_u32 result_u32 = mc_comm_req_size(config);
  const mc_ptr result = mc_comm_init(mc_buffer(alloc_buffer, result_u32.value), config);
  if (MC_SUCCESS != result.error) {
    return result.error;
  }
  mc_comm* const com = result.data;


  char temp[100];
  result_u32 = mc_comm_send(com, temp, sizeof(temp), 10);
  if ((MC_ERR_TIMEOUT != result_u32.error) || (result_u32.value == sizeof(temp))) {
    return MC_ERR_RUNTIME;
  }

  result_u32 = mc_comm_recv(com, temp, sizeof(temp), 10);
  if ((MC_ERR_TIMEOUT != result_u32.error) || (result_u32.value == sizeof(temp))) {
    return MC_ERR_RUNTIME;
  }

  const mc_bool result_bool = mc_comm_flush(com, 1);
  if ((MC_ERR_TIMEOUT != result_bool.error) || (true == result_bool.value)) {
    return MC_ERR_RUNTIME;
  }

  cfg_set_loss_rate(0);
  return MC_SUCCESS;
}

int main()
{
  printf("[MICRO CORE %u.%u.%u - IO - COMMUNICATION]\n", MC_VERSION_MAJOR, MC_VERSION_MINOR, MC_VERSION_PATCH);
  mc_err result = MC_SUCCESS;

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

  printf("[singly_timed_out]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    result = singly_timed_out();
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  return MC_SUCCESS;
}
