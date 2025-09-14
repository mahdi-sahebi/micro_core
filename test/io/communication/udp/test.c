/* For testing this module, we need a non-session based communication
 * like UDP, so we just implement a simple send and receive socket.
 * creation, invalid parameters, diff snd/rcv window sizes, ...
 * 
 * [Test Log]
 * 
[MICRO CORE 1.0.0 - IO - COMMUNICATION]
[invalid_creation]
PASSED - 1(us)

[valid_creation]
PASSED - 1(us)

[singly_direction]
████████████████████ 100.0%
[IO] Completed{Recv: 2471, Send: 2007} - Failed{Recv: 0(0.0%), Send: 0(0.0%)}
PASSED - 3720546(us)

[singly_repetitive]
████████████████████ 100.0%
[IO] Completed{Recv: 2471, Send: 2010} - Failed{Recv: 0(0.0%), Send: 0(0.0%)}
PASSED - 3694864(us)

[singly_low_lossy]
████████████████████ 100.0%
[IO] Completed{Recv: 4495, Send: 4103} - Failed{Recv: 904(20.1%), Send: 836(20.4%)}
PASSED - 3991008(us)

[singly_high_lossy]
████████████████████ 100.0%
[IO] Completed{Recv: 356648, Send: 373906} - Failed{Recv: 339129(95.1%), Send: 355027(95.0%)}
PASSED - 90864987(us)
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


static uint32_t read_api(void* const data, uint32_t size)
{
  return size;
}

static uint32_t write_api(const void* const data, uint32_t size)
{
  return size;
}

static int invalid_creation()
{
  char memory[1024];
  mc_buffer alloc_buffer = mc_buffer(memory, sizeof(memory));
  mc_result_ptr result_ptr = {0};
  mc_comm* message = NULL;
  
  mc_comm_cfg config = mc_comm_cfg(mc_io(NULL, write_api), DATA_LEN * sizeof(uint32_t), 3);
  result_ptr = mc_comm_init(alloc_buffer, config);
  if ((MC_SUCCESS == result_ptr.error) || (NULL != result_ptr.data)) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_comm_cfg(mc_io(read_api, NULL), DATA_LEN * sizeof(uint32_t), 3);
  result_ptr = mc_comm_init(alloc_buffer, config);
  if ((MC_SUCCESS == result_ptr.error) || (NULL != result_ptr.data)) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_comm_cfg(mc_io(read_api, write_api), 0, 3);
  result_ptr = mc_comm_init(alloc_buffer, config);
  if ((MC_SUCCESS == result_ptr.error) || (NULL != result_ptr.data)) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_comm_cfg(mc_io(read_api, write_api), 0, 0);
  result_ptr = mc_comm_init(alloc_buffer, config);
  if ((MC_SUCCESS == result_ptr.error) || (NULL != result_ptr.data)) {
    return MC_ERR_BAD_ALLOC;
  }

  config = mc_comm_cfg(mc_io(read_api, write_api), 1, 0);
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
  const uint32_t capcity = 3;
  
  mc_comm_cfg config = mc_comm_cfg(mc_io(read_api, write_api), 5 * sizeof(uint32_t), capcity);
  const mc_result_ptr result = mc_comm_init(alloc_buffer, config);
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
  const int result = singly_direction();
  cfg_set_repetitive_send(true);
  return result;
}

static int singly_low_lossy()
{
  cfg_set_loss_rate(20);
  cfg_set_iterations(200);
  const int result = singly_direction();
  cfg_set_loss_rate(0);
  return result;
}

static int singly_high_lossy()
{
  cfg_set_loss_rate(95);
  cfg_set_iterations(200);
  const int result = singly_direction();
  cfg_set_loss_rate(0);
  return result;
}

static int full_duplex()
{
  return MC_ERR_RUNTIME;
}

int main()
{
  printf("[MICRO CORE %u.%u.%u - IO - COMMUNICATION]\n", MC_VERSION_MAJOR, MC_VERSION_MINOR, MC_VERSION_PATCH);
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

  /*
  printf("[full_duplex]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    result = full_duplex();
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us) - Recv: %u - Send: %u\n\n", 
        (uint32_t)(mc_now_u() - bgn_time_us), cfg_get_recv_counter(), cfg_get_send_counter());
    }
  }
*/
  return MC_SUCCESS;// TODO(MN): Do && for all results
}
