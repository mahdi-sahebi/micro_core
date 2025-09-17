/* TODO(MN): Check inputs
 * Remove dynamic allocation
 * Remove standard library dependencies as much as possible
 * Define a network interface with write and read handlers
 * define cu32_t? ... 
 * Add crc
 * security: dynamic key authentication, ...
 * test of link: for less than window size
 * link: reliable link + io interface + crc
 * msg: link + authentication + ID based message passing
 * doc: packet order guaranteed
 * Add timeout for recv and send
 * Tests of sending N bytes so that N !+ K*W  W: window size
 * Tests of timeout for send/recv
 * 
 * Doc of update(): is an excellent design for bare-metal and OS compatibility. 
 * can be used inside of a timer callback or thread to not miss any data.
 * Test of 1bit corruption and check test
 * 
 * Flush at the end of recv/send?
 * Get comm interface.
 * Test of incomplete send/recv, packet unlock/lock
 * Use one temp window for send/recv
 * 
 * Doc: Concat the data like TCP.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "core/error.h"
#include "core/time.h"
#include "io/communication/window.h"// TODO(MN): Make it private
#include "io/communication/window_pool.h"
#include "mc_base.h"
#include "mc_io.h"
#include "mc_frame.h"
#include "mc_protocol.h"



#define FRAME_GET_SIZE(WINDOW_SIZE, CAPACITY)\
  (sizeof(mc_frame) + (WINDOW_SIZE) + WNDPOOL_GET_WINDOWS_SIZE(WINDOW_SIZE, CAPACITY))

mc_result_u32 mc_comm_get_alloc_size(mc_comm_cfg config)
{
  if ((NULL == config.io.recv) || (NULL == config.io.send)) {
    return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
  }

  if ((0 == config.recv.capacity) || (0 == config.recv.size) ||
      (0 == config.send.capacity) || (0 == config.send.size)) {
    return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
  }
  if ((config.recv.size < (sizeof(mc_pkt) + 1)) || (config.send.size < (sizeof(mc_pkt) + 1))) {
    return mc_result_u32(0, MC_ERR_BAD_ALLOC);
  }

  const uint32_t recv_frame_size = FRAME_GET_SIZE(config.recv.size, config.recv.capacity);
  const uint32_t send_frame_size = FRAME_GET_SIZE(config.send.size, config.send.capacity);
  const uint32_t frames_size = recv_frame_size + send_frame_size;
  const uint32_t size = sizeof(mc_comm) + frames_size;
  return mc_result_u32(size, MC_SUCCESS);
}

mc_result_ptr mc_comm_init(mc_buffer alloc_buffer, mc_comm_cfg config)
{
  const mc_result_u32 result_u32 = mc_comm_get_alloc_size(config);
  if ((MC_SUCCESS != result_u32.error) || (mc_buffer_get_size(alloc_buffer) < result_u32.value)) {
    return mc_result_ptr(NULL, MC_ERR_BAD_ALLOC);
  }
  
  mc_comm* const this = (mc_comm*)alloc_buffer.data;
  io_init(&this->io, config.io);

  this->rcv = (mc_frame*)((char*)this + sizeof(mc_comm));// TODO(MN): Can be removed and use[0]
  frame_init(this->rcv, config.recv.size, config.recv.capacity);// TODO(MN): Pass mc_comm_wnd

  this->snd = (mc_frame*)((char*)(this->rcv) + FRAME_GET_SIZE(config.recv.size, config.recv.capacity));
  frame_init(this->snd, config.send.size, config.send.capacity);
  
  protocol_init(this);

  return mc_result_ptr(this, MC_SUCCESS);
}

mc_error mc_comm_update(mc_comm* this)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }

  io_recv(this, protocol_recv, this);
  protocol_send_unacked(this);

  return MC_SUCCESS;
}

mc_result_u32 mc_comm_recv(mc_comm* this, void* dst_data, uint32_t size, uint32_t timeout_us)
{
  if ((NULL == this) || (NULL == dst_data)) {
    return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
  }

  uint32_t read_size = 0;
  mc_error error = MC_SUCCESS;
  const mc_time_t end_time = (MC_TIMEOUT_MAX != timeout_us) ? (mc_now_u() + timeout_us) : 0;

  while (size) {
    if ((MC_TIMEOUT_MAX != timeout_us) && (mc_now_u() > end_time)) {
      error = MC_ERR_TIMEOUT;
      break;
    }

    const uint32_t seg_size = wndpool_read(&this->rcv->pool, mc_buffer((char*)dst_data + read_size, size));

    if (seg_size) {
      size -= seg_size;
      read_size += seg_size;
    } else {
      mc_comm_update(this);
      usleep(MIN_SEND_TIME_US);
    }
  }

  return mc_result_u32(read_size, error);
}

mc_result_u32 mc_comm_send(mc_comm* this, const void* src_data, uint32_t size, uint32_t timeout_us)
{
  if ((NULL == this) || (NULL == src_data)) {
    return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
  }

  uint32_t sent_size = 0;
  mc_error error = MC_SUCCESS;
  const mc_time_t end_time = (MC_TIMEOUT_MAX != timeout_us) ? (mc_now_u() + timeout_us) : 0;

  while (size) {
    const uint32_t seg_size = MIN(size, this->snd->pool.window_size - sizeof(mc_pkt));
    mc_buffer buffer = mc_buffer((char*)src_data + sent_size, seg_size);
    buffer = protocol_send(this, buffer);
    
    if (0 != buffer.capacity) {
      size -= seg_size;
      sent_size += seg_size;
    } else {
      mc_comm_update(this);
      usleep(MIN_SEND_TIME_US);
    }
    
    if ((MC_TIMEOUT_MAX != timeout_us) && (mc_now_u() > end_time)) {
      error = MC_ERR_TIMEOUT;
      break;
    }
  }
  
  return mc_result_u32(sent_size, error);
}

mc_result_bool mc_comm_flush(mc_comm* this, uint32_t timeout_us)
{
  if (NULL == this) {
    return mc_result_bool(false, MC_ERR_INVALID_ARGUMENT);
  }

  const mc_time_t end_time_us = mc_now_u() + timeout_us;

  while (!wndpool_is_empty(&this->snd->pool) || !wndpool_is_empty(&this->rcv->pool)) {
    mc_comm_update(this);

    if (mc_now_u() > end_time_us) {
      return mc_result_bool(false, MC_ERR_TIMEOUT);
    }

    usleep(MIN_SEND_TIME_US);
  }
  
  return mc_result_bool(true, MC_SUCCESS);
}

#undef FRAME_GET_SIZE
#undef MAX_SEND_TIME_US
#undef MIN_SEND_TIME_US
#undef MIN
#undef MAX
