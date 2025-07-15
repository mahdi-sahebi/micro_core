/* For testing this module, we need a non-session based communication
 * like UDP, so we just implement a simple send and receive socket.
 * creation, invalid parameters, diff snd/rcv window sizes, ...
 * 
 * [Test Log]
 * 
  [MICRO CORE 1.0.0 - IO - MESSAGE]
  [invalid_creation]
  PASSED - 1(us)

  [valid_creation]
  PASSED - 6(us)

  [singly_direction]
  ████████████████████ 100.0%
  Completed[Recv: 20000, Send: 20000] - Failed[Recv: 0(0.0%), Send: 0(0.0%)]

  PASSED - 7483800(us)

  [singly_repetitive]
  ████████████████████ 100.0%
  Completed[Recv: 20000, Send: 20000] - Failed[Recv: 0(0.0%), Send: 0(0.0%)]

  PASSED - 7494000(us)

  [singly_low_lossy]
  ████████████████████ 100.0%
  Completed[Recv: 5810, Send: 5801] - Failed[Recv: 1427(19.7%), Send: 1521(20.8%)]

  PASSED - 5786943(us)

  [singly_high_lossy]
  ████████████████████ 100.0%
  Completed[Recv: 4281, Send: 4281] - Failed[Recv: 213604(98.0%), Send: 79823(94.9%)]

  PASSED - 45213009(us)
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "core/error.h"
#include "core/version.h"
#include "core/time.h"
#include "io/message/message.h"
#include "test_message_udp_common.h"
#include "test_message_udp_receiver.h"
#include "test_message_udp_sender.h"


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
  cfg_set_iterations(10000);
  const int result = singly_direction();
  cfg_set_repetitive_send(true);
  return result;
}

static int singly_low_lossy()
{
  cfg_set_loss_rate(20);
  cfg_set_iterations(1000);
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
  printf("[small_write]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    result = small_write();
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us) - Recv: %u - Send: %u\n\n", 
        (uint32_t)(mc_now_u() - bgn_time_us), cfg_get_recv_counter(), cfg_get_send_counter());
    }
  }


  printf("[large_write]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    result = large_write();
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us) - Recv: %u - Send: %u\n\n", 
        (uint32_t)(mc_now_u() - bgn_time_us), cfg_get_recv_counter(), cfg_get_send_counter());
    }
  }


  printf("[hulf_duplex]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    result = hulf_duplex();
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us) - Recv: %u - Send: %u\n\n", 
        (uint32_t)(mc_now_u() - bgn_time_us), cfg_get_recv_counter(), cfg_get_send_counter());
    }
  }


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
  return MC_SUCCESS;
}
