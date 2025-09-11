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

typedef struct __attribute__((packed))
{
  // TODO(mn): pad
  uint16_t  temp_stored;// TODO(MN): Check max temp size
  mc_pkt*   temp_window;
  wndpool_t pool;
}mc_frame;

#define MC_FRAME_GET_SIZE(WINDOW_SIZE, CAPACITY)\
  (sizeof(mc_frame) + (WINDOW_SIZE) + WNDPOOL_GET_WINDOWS_SIZE(WINDOW_SIZE, CAPACITY))

struct _mc_comm_t
{ 
  mc_frame* rcv;// TODO(MN): Use array to reduce one pointer size
  mc_frame* snd;
  mc_io     io;
  uint32_t  send_delay_us;// TODO(MN): Use u16 with 100X us resolution
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
  pkt->crc    = mc_alg_crc16_ccitt(mc_buffer(pkt, this->snd->pool.window_size)).value;

  send_buffer(this, pkt, this->rcv->pool.window_size);
}

static mc_buffer protocol_send(mc_comm* this, mc_buffer buffer)
{
  mc_comm_update(this);
  return buffer;
}

static mc_buffer protocol_recv(mc_comm* this, mc_buffer buffer)
{
  mc_pkt* const pkt = (mc_pkt*)buffer.data;

  if (PKT_ACK == pkt->type) {
    if (!wndpool_contains(&this->snd->pool, pkt->id)) {// TODO(MN): Test that not read to send ack to let sender sends more
      return mc_buffer(NULL, 0);
    }
    // TODO(MN): Not per ack
    const uint64_t elapsed_time = mc_now_u() - wndpool_get(&this->snd->pool, pkt->id)->sent_time_us;
    this->send_delay_us = MIN(MAX(elapsed_time * 0.8, MIN_SEND_TIME_US), MAX_SEND_TIME_US);
    wndpool_ack(&this->snd->pool, pkt->id);
    return mc_buffer(NULL, 0);// done
  }

  if (pkt->id < this->rcv->pool.bgn_id) {// TODO(MN): Handle overflow
    send_ack(this, pkt->id);
    return mc_buffer(NULL, 0);// done
  }

  if (wndpool_update(&this->rcv->pool, mc_buffer(pkt->data, pkt->size), pkt->id)) {
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
  const uint32_t size = wndpool_write(&this->snd->pool, buffer, on_send_window_ready, this);
  return mc_buffer(buffer.data, size);
}

static void frame_init(mc_frame* frame, uint16_t window_size, uint8_t capacity)
{
  wndpool_init(&frame->pool, window_size, capacity);
  frame->temp_stored = 0;
  frame->temp_window = (mc_pkt*)((char*)frame->pool.windows + WNDPOOL_GET_WINDOWS_SIZE(window_size, capacity));
}

static bool frame_is_completed(mc_frame* frame)
{
  return (frame->temp_stored == frame->pool.window_size);
}

static bool frame_is_header_valid(const mc_pkt* const pkt)
{
   return (HEADER == pkt->header);
}

static void frame_drop(const mc_pkt* const pkt)
{
   // TODO(MN): Drop the data until find correct header
}

static bool frame_is_crc_valid(wndpool_t* pool, mc_pkt* pkt)
{
  const uint16_t received_crc = pkt->crc;
  pkt->crc = 0x0000;
  const uint16_t crc = mc_alg_crc16_ccitt(mc_buffer(pkt, pool->window_size)).value;
  return (received_crc == crc);
}

static void frame_recv(mc_comm* this)
{
  if (!frame_is_completed(this->rcv)) {
    return;
  }

  this->rcv->temp_stored = 0;
  mc_pkt* const pkt = (mc_pkt*)this->rcv->temp_window;

  if (!frame_is_header_valid(pkt)) {// TODO(MN): Packet unlocked. Find header. simulated in tests to unlock
    frame_drop(pkt);
    return;
  }
  if (!frame_is_crc_valid(&this->rcv->pool, pkt)) {// Data corruption
    return;
  }

  // TODO(MN): Callback
  protocol_recv(this, mc_buffer(this->rcv->temp_window, this->rcv->pool.window_size));
}

static void io_recv(mc_comm* this)
{
  const uint32_t required_size = this->rcv->pool.window_size - this->rcv->temp_stored;
  void* const temp_buffer = (char*)this->rcv->temp_window + this->rcv->temp_stored;
  const uint32_t read_size = this->io.recv(temp_buffer, required_size);
  
  if (0 != read_size) {
    this->rcv->temp_stored += read_size;
    frame_recv(this); 
  }
}

static mc_buffer pipeline_send(mc_comm* this, mc_buffer buffer)
{
  buffer = protocol_send(this, buffer);
  if (NULL == buffer.data) {
    return buffer;
  }

  buffer = frame_send(this, buffer);
  return buffer;
}

static void send_unacked(mc_comm* const this) 
{
  const mc_time_t now = mc_now_u();

  for (mc_pkt_id id = this->snd->pool.bgn_id; id < this->snd->pool.end_id; id++) {
    wnd_t* const window = wndpool_get(&this->snd->pool, id);
    if (wnd_is_acked(window) || (now < (window->sent_time_us + this->send_delay_us))) {
      continue;
    }

    if (send_buffer(this, &window->packet, this->snd->pool.window_size)) {
      // window->sent_time_us = mc_now_u();
    }
  }


  if (!wndpool_is_empty(&this->snd->pool)) {
    if (mc_now_m() > (this->snd->pool.update_time + 1000)) {
      wndpool_update_header(&this->snd->pool);

      wnd_t* const window = wndpool_get(&this->snd->pool, this->snd->pool.end_id);// TODO(MN): Use index
      mc_buffer last_buffer = mc_buffer(&window->packet, this->snd->pool.window_size);
      on_send_window_ready(last_buffer, this);
    }
  }
}

mc_result_u32 mc_comm_get_alloc_size(uint16_t window_size, uint8_t capacity)
{
  if ((0 == capacity) || (0 == window_size)) {
    return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
  }
  if ((capacity >= (1 << (sizeof(mc_wnd_idx) * 8))) || (window_size < (sizeof(mc_pkt) + 1))) {
    return mc_result_u32(0, MC_ERR_BAD_ALLOC);
  }

  const uint32_t frames_size = 2 * MC_FRAME_GET_SIZE(window_size, capacity);
  const uint32_t size = sizeof(mc_comm) + frames_size;
  return mc_result_u32(size, MC_SUCCESS);
}

mc_result_ptr mc_comm_init(
  mc_buffer alloc_buffer,
  uint16_t window_size, 
  uint8_t capacity, 
  mc_io io)
{
  if ((NULL == io.recv) || (NULL == io.send)) {
    return mc_result_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
  }

  const mc_result_u32 result_u32 = mc_comm_get_alloc_size(window_size, capacity);
  if ((MC_SUCCESS != result_u32.error) || (mc_buffer_get_size(alloc_buffer) < result_u32.value)) {
    return mc_result_ptr(NULL, MC_ERR_BAD_ALLOC);
  }
  
  mc_comm* const this = (mc_comm*)alloc_buffer.data;
  this->io            = io;
  this->send_delay_us = MIN_SEND_TIME_US;

  this->rcv = (mc_frame*)((char*)this + sizeof(mc_comm));// TODO(MN): Can be removed and use[0]
  frame_init(this->rcv, window_size, capacity);

  this->snd = (mc_frame*)((char*)(this->rcv) + MC_FRAME_GET_SIZE(window_size, capacity));
  frame_init(this->snd, window_size, capacity);

  return mc_result_ptr(this, MC_SUCCESS);
}

mc_error mc_comm_update(mc_comm* this)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }

  io_recv(this);
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

  while (!wndpool_is_empty(&this->snd->pool) || !wndpool_is_empty(&this->rcv->pool)) {
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
