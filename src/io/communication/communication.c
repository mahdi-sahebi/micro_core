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
 * 
 * 
 * Get comm interface.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "core/error.h"
#include "core/time.h"
#include "io/communication/window.h"
#include "io/communication/window_pool.h"
#include "io/communication/communication.h"


#define MAX_SEND_TIME_US    3000000
#define MIN_SEND_TIME_US    100
#define MIN(A, B)           ((A) <= (B) ? (A) : (B))
#define MAX(A, B)           ((A) >= (B) ? (A) : (B))

struct _mc_comm_t
{ 
  mc_io      io;
  uint32_t   send_delay_us;
  wndpool_t* rcv;// TODO(MN): Use array to reduce one pointer size
  wndpool_t* snd;
};


static void send_ack(mc_comm_t* const this, uint32_t id)
{
  pkt_t* const pkt = this->snd->temp_window;

  pkt->header = HEADER;
  pkt->type   = PKT_ACK;
  pkt->id     = id;
  
  if (this->io.send(pkt, this->rcv->window_size) != this->rcv->window_size) {
    // TODO(MN): Handle. Is it ok to 
  }
  // ("[PACKET %u] Sent ACK (Total ACKs sent: %u)\n",         seq, total_packets_received);
}

static uint32_t read_data(mc_comm_t* const this)
{
  pkt_t* const pkt = this->rcv->temp_window;
  const uint32_t read_size = this->io.recv(pkt, this->rcv->window_size);
  if (0 == read_size) {// TODO(MN): Handle incomplete size(smaller or larger)
    return 0;
  }

  if (HEADER != pkt->header) {// TODO(MN): Packet unlocked. Find header
    return 0; // [INVALID] Bad header/type received. 
  }
 
  if (PKT_ACK == pkt->type) {
    if (!wndpool_contains(this->snd, pkt->id)) {// TODO(MN): Test that not read to send ack to let sender sends more
      return 0;
    }

    const mc_time_t sent_time_us = wndpool_get(this->snd, pkt->id)->sent_time_us;
    const uint64_t elapsed_time = mc_now_u() - sent_time_us;
    this->send_delay_us = elapsed_time * 0.8;
    wndpool_ack(this->snd, pkt->id, NULL);
    return read_size;
  }

  if (pkt->id < this->rcv->bgn_id) {// TODO(MN): Handle overflow
    send_ack(this, pkt->id);
    return 0;
  }

  if (wndpool_insert(this->rcv, mc_span(pkt->data, pkt->size), pkt->id)) {
    send_ack(this, pkt->id);
  }

  return read_size;
}

static void send_unacked(mc_comm_t* const this) 
{
  const uint32_t end_id = this->snd->bgn_id + this->snd->capacity;

  for (uint32_t id = this->snd->bgn_id; id < end_id; id++) {
    wnd_t* const window = wndpool_get(this->snd, id);
    if (!wnd_is_valid(window) ||
        wnd_is_acked(window) || 
        !wnd_is_timedout(window, this->send_delay_us)) {
      continue;
    }

    const uint32_t sent_size = this->io.send(&window->packet, this->snd->window_size);
    if (this->snd->window_size == sent_size) {
      window->sent_time_us = mc_now_u();
    }// TODO(MN): Handle if send is incomplete. attempt 3 times! 
  }
}

mc_result_u32 mc_comm_get_alloc_size(uint16_t window_size, uint8_t window_capacity)
{
  if ((0 == window_capacity) || (0 == window_size)) {
    return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
  }
  if ((window_capacity >= (1 << (sizeof(mc_comm_idx) * 8))) || (window_size < (sizeof(pkt_t) + 1))) {
    return mc_result_u32(0, MC_ERR_BAD_ALLOC);
  }

  const uint32_t windows_size = window_capacity * (sizeof(wnd_t) + window_size);
  /*                                                         temp window + all windows */
  const uint32_t controllers_size = 2 * (sizeof(wndpool_t) + window_size + windows_size);
  const uint32_t size = sizeof(mc_comm_t) + controllers_size;
  return mc_result_u32(size, MC_SUCCESS);
}

mc_comm_t* mc_comm_init(
  mc_span alloc_buffer,
  uint16_t window_size, 
  uint8_t window_capacity, 
  mc_io io)
{
  if ((NULL == io.recv) || (NULL == io.send)) {
    return NULL;// TODO(MN): MC_ERR_INVALID_ARGUMENT;
  }

  const mc_result_u32 result_u32 = mc_comm_get_alloc_size(window_size, window_capacity);
  if ((MC_SUCCESS != result_u32.result) || (mc_span_get_size(alloc_buffer) < result_u32.value)) {
    return NULL;// TODO(MN): MC_ERR_BAD_ALLOC
  }
  
  const uint32_t windows_size = window_capacity * (sizeof(wnd_t) + window_size);
  mc_comm_t* const this = (mc_comm_t*)alloc_buffer.data;

  this->io               = io;
  this->send_delay_us    = MIN_SEND_TIME_US;

  this->rcv              = (wndpool_t*)((char*)this + sizeof(mc_comm_t));
  this->rcv->window_size = window_size;
  this->rcv->data_size   = window_size - sizeof(pkt_t);
  this->rcv->capacity    = window_capacity;
  this->rcv->windows     = (wnd_t*)((char*)this->rcv->temp_window + window_size);

  this->snd              = (wndpool_t*)(char*)(this->rcv->windows) + windows_size;
  this->snd->window_size = window_size;
  this->snd->data_size   = window_size - sizeof(pkt_t);
  this->snd->capacity    = window_capacity;
  this->snd->windows     = (wnd_t*)((char*)this->snd->temp_window + window_size);

  wndpool_clear(this->rcv);
  wndpool_clear(this->snd);
  
  return this;
}

mc_error mc_comm_update(mc_comm_t* this)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }

  read_data(this);
  send_unacked(this);
  return MC_SUCCESS;
}

uint32_t mc_comm_recv(mc_comm_t* this, void* data, uint32_t size, uint32_t timeout_us)
{
  uint32_t read_size = 0;
  const mc_time_t bgn_time = (MC_TIMEOUT_MAX != timeout_us) ? mc_now_u() : 0;

  while (size) {
    mc_comm_update(this);

    const uint32_t seg_size = wndpool_pop(this->rcv, (char*)data + read_size, size);

    size -= seg_size;
    read_size += seg_size;

    if ((MC_TIMEOUT_MAX != timeout_us) && (mc_now_u() > (bgn_time + timeout_us))) {
      break;// TODO(MN): Error of timeout
    }
  }

  return read_size;
}

uint32_t mc_comm_send(mc_comm_t* this, const void* data, uint32_t size, uint32_t timeout_us)
{
  uint32_t sent_size = 0;
  const mc_time_t bgn_time = (MC_TIMEOUT_MAX != timeout_us) ? mc_now_u() : 0;

  while (size) {
    const uint32_t seg_size = MIN(size, this->snd->window_size - sizeof(pkt_t));

    const wnd_t* const window = wndpool_get(this->snd, this->snd->end_id);// TODO(MN): Bad design
    if (wndpool_push(this->snd, mc_span((char*)data + sent_size, seg_size))) { // TODO(MN): Don't Send incompleted windows, allow further sends attach their data
      this->io.send(&window->packet, this->snd->window_size); // TODO(MN): Handle incomplete sending. also handle a timeout if fails continuously

      size -= seg_size;// TODO(MN): API for + and - in span
      sent_size += seg_size;
    } else {
      mc_comm_update(this);// TODO(MN): pure function without any check
    }
    
    if ((MC_TIMEOUT_MAX != timeout_us) && (mc_now_u() > (bgn_time + timeout_us))) {
      break;// TODO(MN): Error of timeout
    }
  }
  
  return sent_size;
}

bool mc_comm_flush(mc_comm_t* this, uint32_t timeout_us)
{
  const mc_time_t bgn_time_us = mc_now_u();

  while (!wndpool_is_empty(this->snd) || !wndpool_is_empty(this->rcv)) {
    mc_comm_update(this);

    if ((mc_now_u() - bgn_time_us) > timeout_us) {
      return false;
    }
  }

  return true;
}


#undef MAX_SEND_TIME_US
#undef MIN_SEND_TIME_US
#undef MIN
#undef MAX
