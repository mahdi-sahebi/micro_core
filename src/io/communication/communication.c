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
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "core/error.h"
#include "core/time.h"
#include "pattern/mc_chain.h"
#include "io/communication/window.h"
#include "io/communication/window_pool.h"
#include "mc_transceiver.h"
#include "mc_frame.h"
#include "mc_protocol.h"
#include "io/communication/communication.h"


#define MAX_SEND_TIME_US    3000000
#define MIN_SEND_TIME_US    100
// TODO(MN): Move to math module
#define MIN(A, B)           ((A) <= (B) ? (A) : (B))
#define MAX(A, B)           ((A) >= (B) ? (A) : (B))


mc_result_u32 mc_comm_get_alloc_size(uint16_t window_size, uint8_t windows_capacity)
{
  if ((0 == windows_capacity) || (0 == window_size)) {
    return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
  }
  if ((windows_capacity >= (1 << (sizeof(mc_wnd_idx) * 8))) || (window_size < (sizeof(mc_pkt) + 1))) {
    return mc_result_u32(0, MC_ERR_BAD_ALLOC);
  }

  const uint32_t windows_size = windows_capacity * wnd_get_size(window_size);
  /*                                                         temp window + all windows */
  const uint32_t controllers_size = 2 * (sizeof(wndpool_t) + window_size + windows_size);
  const uint32_t chains_size = mc_chain_get_alloc_size(3).value * 2;
  const uint32_t size = sizeof(mc_comm) + controllers_size + chains_size;
  return mc_result_u32(size, MC_SUCCESS);
}

mc_result_ptr mc_comm_init(
  mc_span alloc_buffer,
  uint16_t window_size, 
  uint8_t windows_capacity, 
  mc_io io)
{
  if ((NULL == io.recv) || (NULL == io.send)) {
    return mc_result_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
  }

  const mc_result_u32 result_u32 = mc_comm_get_alloc_size(window_size, windows_capacity);
  if ((MC_SUCCESS != result_u32.result) || (mc_span_get_size(alloc_buffer) < result_u32.value)) {
    return mc_result_ptr(NULL, MC_ERR_BAD_ALLOC);
  }
  
  const uint32_t windows_size = windows_capacity * wnd_get_size(window_size);
  mc_comm* const this = (mc_comm*)alloc_buffer.data;

  this->io               = io;
  this->send_delay_us    = MIN_SEND_TIME_US;

  // TODO(MN): Define all buffers
  this->rcv              = (wndpool_t*)((char*)this + sizeof(mc_comm));// TODO(MN): Can be removed and use[0]
  this->rcv->window_size = window_size;
  this->rcv->capacity    = windows_capacity;
  this->rcv->windows     = (wnd_t*)((char*)this->rcv->temp_window + window_size);

  this->snd              = (wndpool_t*)(char*)(this->rcv->windows) + windows_size;
  this->snd->window_size = window_size;
  this->snd->capacity    = windows_capacity;
  this->snd->windows     = (wnd_t*)((char*)this->snd->temp_window + window_size);

  const mc_span recv_chain_buffer = mc_span(this->snd->windows, windows_size);
  const mc_span send_chain_buffer = mc_span(mc_span_end(recv_chain_buffer), mc_chain_get_alloc_size(3).value);

  wndpool_clear(this->rcv);
  wndpool_clear(this->snd);
  
  mc_result_ptr result = {0};
  result = mc_chain_init(recv_chain_buffer, 3);
  if (MC_SUCCESS != result.result) {
    return mc_result_ptr(NULL, result.result);
  }
  this->recv_chain = (mc_chain*)result.data;

  result = mc_chain_init(send_chain_buffer, 3);
  if (MC_SUCCESS != result.result) {
    return mc_result_ptr(NULL, result.result);
  }
  this->send_chain = (mc_chain*)result.data;

  mc_chain_push(this->recv_chain, mc_transceiver_recv, &this->io);
  mc_chain_push(this->recv_chain, mc_frame_recv, this->rcv);
  mc_chain_push(this->recv_chain, mc_protocol_recv, this);
  
  mc_chain_push(this->send_chain, mc_protocol_send, this);
  mc_chain_push(this->send_chain, mc_frame_send, this->snd);
  mc_chain_push(this->send_chain, mc_transceiver_send, &this->io);

  return mc_result_ptr(this, MC_SUCCESS);
}

mc_error mc_comm_update(mc_comm* this)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }

  const mc_chain_data data = mc_chain_run(this->recv_chain, mc_span(this->rcv->temp_window, this->rcv->window_size));
  // send_unacked(this);

  return data.error;
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
    mc_comm_update(this);
    const uint32_t seg_size = wndpool_pop(this->rcv, (char*)dst_data + read_size, size);

    // TODO(MN): Style not equals to send
    size -= seg_size;
    read_size += seg_size;

    if ((MC_TIMEOUT_MAX != timeout_us) && (mc_now_u() > end_time)) {
      error = MC_ERR_TIMEOUT;
      break;
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
    const uint32_t seg_size = MIN(size, this->snd->window_size - sizeof(mc_pkt));
    mc_span buffer = mc_span((char*)src_data + sent_size, seg_size);

    mc_comm_update(this);
    mc_chain_data data = mc_chain_run(this->send_chain, buffer);
    
    if (MC_SUCCESS == data.error){
      size -= seg_size;
      sent_size += seg_size;
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

  while (wndpool_get_count(this->snd) || wndpool_get_count(this->rcv)) {
    mc_comm_update(this);

    if (mc_now_u() > end_time_us) {
      return mc_result_bool(false, MC_ERR_TIMEOUT);
    }
  }
  
  return mc_result_bool(true, MC_SUCCESS);
}


#undef MAX_SEND_TIME_US
#undef MIN_SEND_TIME_US
#undef MIN
#undef MAX
