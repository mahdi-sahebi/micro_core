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
#include "io/communication/window.h"
#include "io/communication/window_pool.h"
#include "io/communication/communication.h"


#define MAX_SEND_TIME_US    3000000
#define MIN_SEND_TIME_US    100
// TODO(MN): Move to math module
#define MIN(A, B)           ((A) <= (B) ? (A) : (B))
#define MAX(A, B)           ((A) >= (B) ? (A) : (B))

struct _mc_comm_t
{ 
  wndpool_t* rcv;// TODO(MN): Use array to reduce one pointer size
  wndpool_t* snd;
  mc_io      io;
  uint32_t   send_delay_us;// TODO(MN): Use u16 with 100X us resolution
};



static bool send_buffer(mc_comm* this, const void* buffer, uint32_t size)
{
  uint8_t index = 3;
  while (index--) {
    if (size == this->io.send(buffer, size)) {
      return true;
    }// TODO(MN): Handle if send is incomplete. attempt 3 times! 

    usleep(100);
  }
  
  return false;
}

static void send_ack(mc_comm* this, uint32_t id)
{
  mc_pkt* const pkt = this->snd->temp_window;

  pkt->header = HEADER;
  pkt->type   = PKT_ACK;
  pkt->id     = id;
  pkt->crc    = 0x0000;
  pkt->crc    = mc_alg_crc16_ccitt(mc_buffer(pkt, this->snd->window_size)).value;

  send_buffer(this, pkt, this->rcv->window_size);
}

static mc_buffer protocol_send(mc_comm* this, mc_buffer buffer)
{
  mc_comm_update(this);
  return buffer;
}

static mc_buffer protocol_recv(mc_comm* this, mc_buffer buffer)
{
  if (buffer.capacity != this->rcv->window_size) {// TODO(MN): Full check
    return mc_buffer(NULL, 0);
  }

  mc_pkt* const pkt = (mc_pkt*)buffer.data;

  if (PKT_ACK == pkt->type) {
    if (!wndpool_contains(this->snd, pkt->id)) {// TODO(MN): Test that not read to send ack to let sender sends more
      return mc_buffer(NULL, 0);
    }
    // TODO(MN): Not per ack
    const uint64_t elapsed_time = mc_now_u() - wndpool_get(this->snd, pkt->id)->sent_time_us;
    this->send_delay_us = MIN(MAX(elapsed_time * 0.8, MIN_SEND_TIME_US), MAX_SEND_TIME_US);
    wndpool_ack(this->snd, pkt->id);
    return mc_buffer(NULL, 0);// done
  }

  if (pkt->id < this->rcv->bgn_id) {// TODO(MN): Handle overflow
    send_ack(this, pkt->id);
    return mc_buffer(NULL, 0);// done
  }

  if (wndpool_update(this->rcv, mc_buffer(pkt->data, pkt->size), pkt->id)) {
    send_ack(this, pkt->id);
  }

  return buffer;
}

static void on_send_window_ready(mc_buffer buffer, void* arg)
{
  mc_comm* this = arg;
  send_buffer(this, buffer.data, buffer.capacity);
}

static mc_buffer frame_send(mc_comm* this, mc_buffer buffer)
{
  const uint32_t size = wndpool_write(this->snd, buffer, on_send_window_ready, this);
  return mc_buffer(buffer.data, size);
  // const wnd_t* const window = wndpool_get(this->snd, this->snd->end_id);// TODO(MN): Bad design
  // if (wndpool_push(this->snd, buffer)) { // TODO(MN): Don't Send incompleted windows, allow further sends attach their data
  //   return mc_buffer(&window->packet, this->snd->window_size);
  // }

  // return mc_buffer(NULL, 0);
}

static mc_buffer frame_recv(mc_comm* this, mc_buffer buffer)
{
  if (buffer.capacity != this->rcv->window_size) {// TODO(MN): Full check
    return mc_buffer(NULL, 0);
  }

  mc_pkt* const pkt = (mc_pkt*)buffer.data;
  if (HEADER != pkt->header) {// TODO(MN): Packet unlocked. Find header
    return mc_buffer(NULL, 0); // [INVALID] Bad header/type received. 
  }

  const uint16_t received_crc = pkt->crc;
  pkt->crc = 0x0000;
  const uint16_t crc = mc_alg_crc16_ccitt(mc_buffer(pkt, this->rcv->window_size)).value;
  if (received_crc != crc) {
    return mc_buffer(NULL, 0);// Data corruption
  }

  return buffer;
}

static mc_buffer io_send(mc_comm* this, mc_buffer buffer)
{
  const uint32_t sent_size = send_buffer(this, buffer.data, buffer.capacity) ? buffer.capacity : 0;
  return mc_buffer(buffer.data, sent_size);
}

static mc_buffer io_recv(mc_comm* this, mc_buffer buffer)
{
  const uint32_t read_size = this->io.recv(buffer.data, buffer.capacity);
  return mc_buffer(buffer.data, read_size);
}

static mc_buffer pipeline_send(mc_comm* this, mc_buffer buffer)
{
  buffer = protocol_send(this, buffer);
  if (NULL == buffer.data) {
    return buffer;
  }

  buffer = frame_send(this, buffer);
  // if (NULL == buffer.data) {
  //   return buffer;
  // }

  // buffer = io_send(this, buffer);
  return buffer;
}

static mc_buffer pipeline_recv(mc_comm* this, mc_buffer buffer)
{
  buffer = io_recv(this, buffer);
  if (NULL == buffer.data) {
    return buffer;
  }

  buffer = frame_recv(this, buffer);
  if (NULL == buffer.data) {
    return buffer;
  }

  buffer = protocol_recv(this, buffer);
  return buffer;
}

static void send_unacked(mc_comm* const this) 
{
  const mc_time_t now = mc_now_u();

  for (mc_pkt_id id = this->snd->bgn_id; id < this->snd->end_id; id++) {
    wnd_t* const window = wndpool_get(this->snd, id);
    if (wnd_is_acked(window) || (now < (window->sent_time_us + this->send_delay_us))) {
      continue;
    }

    if (send_buffer(this, &window->packet, this->snd->window_size)) {
      // window->sent_time_us = mc_now_u();
    }
  }
}

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
  const uint32_t size = sizeof(mc_comm) + controllers_size;
  return mc_result_u32(size, MC_SUCCESS);
}

mc_result_ptr mc_comm_init(
  mc_buffer alloc_buffer,
  uint16_t window_size, 
  uint8_t windows_capacity, 
  mc_io io)
{
  if ((NULL == io.recv) || (NULL == io.send)) {
    return mc_result_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
  }

  const mc_result_u32 result_u32 = mc_comm_get_alloc_size(window_size, windows_capacity);
  if ((MC_SUCCESS != result_u32.error) || (mc_buffer_get_size(alloc_buffer) < result_u32.value)) {
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
  this->rcv->windows     = (wnd_t*)((char*)(this->rcv->temp_window) + window_size);

  this->snd              = (wndpool_t*)((char*)(this->rcv->windows) + windows_size);
  this->snd->window_size = window_size;
  this->snd->capacity    = windows_capacity;
  this->snd->windows     = (wnd_t*)((char*)(this->snd->temp_window) + window_size);

  wndpool_clear(this->rcv);
  wndpool_clear(this->snd);

  return mc_result_ptr(this, MC_SUCCESS);
}

mc_error mc_comm_update(mc_comm* this)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }

  pipeline_recv(this, mc_buffer(this->rcv->temp_window, this->rcv->window_size));
  send_unacked(this);

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

    const uint32_t seg_size = wndpool_read(this->rcv, mc_buffer((char*)dst_data + read_size, size));

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
    const uint32_t seg_size = MIN(size, this->snd->window_size - sizeof(mc_pkt));
    mc_buffer buffer = mc_buffer((char*)src_data + sent_size, seg_size);
    buffer = pipeline_send(this, buffer);
    
    // if ((NULL != buffer.data)) {
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

  while (wndpool_get_count(this->snd) || wndpool_get_count(this->rcv)) {
    mc_comm_update(this);

    if (mc_now_u() > end_time_us) {
      return mc_result_bool(false, MC_ERR_TIMEOUT);
    }

    usleep(MIN_SEND_TIME_US);
  }
  
  return mc_result_bool(true, MC_SUCCESS);
}


#undef MAX_SEND_TIME_US
#undef MIN_SEND_TIME_US
#undef MIN
#undef MAX
