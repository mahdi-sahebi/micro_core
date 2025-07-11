/* TODO(MN): Check inputs
 * Remove dynamic allocation
 * Remove standard library dependencies as much as possible
 * Define a network interface with write and read handlers
 * define cu32_t? ... 
 * Add crc
 * security: dynamic key authentication, ...
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "core/error.h"
#include "core/time.h"
#include "io/message/window.h"
#include "io/message/window_pool.h"
#include "io/message/message.h"


#define MAX_TIMEOUT_US    3000000

struct _mc_msg_t
{ 
  mc_msg_read_fn       read;
  mc_msg_write_fn      write;
  mc_msg_on_receive_fn on_receive;
  wndpool_t*           rcv;// TODO(MN): Use array to reduce one pointer size
  wndpool_t*           snd;  
};


static void send_ack(mc_msg_t* const this, uint32_t id)
{
  pkt_t* const pkt = this->snd->temp_window;

  pkt->header = HEADER;
  pkt->type   = PKT_ACK;
  pkt->id     = id;
  
  if (this->write(pkt, this->rcv->window_size) != this->rcv->window_size) {
    // TODO(MN): Handle. Is it ok to 
  }
  // ("[PACKET %u] Sent ACK (Total ACKs sent: %u)\n",         seq, total_packets_received);
}

static uint32_t read_data(mc_msg_t* const this)
{
  pkt_t* const pkt = this->rcv->temp_window;
  const uint32_t read_size = this->read(pkt, this->rcv->window_size);
  if (0 == read_size) {// TODO(MN): Handle incomplete size(smaller or larger)
    return 0;
  }

  if (HEADER != pkt->header) {// TODO(MN): Packet unlocked. Find header
    return 0; // [INVALID] Bad header/type received. 
  }

  if (PKT_ACK == pkt->type) {
    wndpool_ack(this->snd, pkt->id, NULL);
    return read_size;
  }

  if (pkt->id < this->rcv->bgn_id) {// TODO(MN): Handle overflow
    send_ack(this, pkt->id);
    return 0;
  }

  if (pkt->id > this->rcv->bgn_id) {
    wndpool_insert(this->rcv, mc_span(pkt->data, pkt->size), pkt->id);
    return read_size;
  }

  send_ack(this, pkt->id);
  wndpool_remove_first(this->rcv);
  this->on_receive(pkt->data, pkt->size);
  wndpool_remove_acked(this->rcv, this->on_receive);// TODO(MN): Merge with wndpool_ack

  return read_size;
}

static void send_unacked(mc_msg_t* const this) 
{
  const uint32_t end_id = this->snd->bgn_id + this->snd->capacity;

  for (uint32_t id = this->snd->bgn_id; id < end_id; id++) {
    const wnd_t* const window = wndpool_get(this->snd, id);
    if (!wnd_is_valid(window)) {
      continue;
    }
    
    if (wnd_is_acked(window) || !wnd_is_timedout(window, MAX_TIMEOUT_US)) {// TODO(MN): Check timeout occurance
        continue;
    }
    
    if (0 != this->write(&window->packet, this->snd->window_size)) {
    }// TODO(MN): Handle if send is incomplete. attempt 3 times! 
  }
}

mc_msg_t* mc_msg_new(
  mc_msg_read_fn read_fn, 
  mc_msg_write_fn write_fn, 
  uint32_t window_size, 
  uint8_t capacity, 
  mc_msg_on_receive_fn on_receive)
{
  // TODO(MN): Input checking. the minimum size of window_size
  if ((NULL == read_fn) || (NULL == write_fn) ||
      (0 == window_size) || (0 == capacity) || 
      (capacity >= (sizeof(idx_t) * 8))) {
    return NULL;// TODO(MN): MC_ERR_INVALID_ARGUMENT;
  }

  if (window_size < (sizeof(pkt_t) + 1)) {
    return NULL;//MC_ERR_OUT_OF_RANGE;
  }
  
  const uint32_t windows_size = capacity * (sizeof(wnd_t) + window_size);
  /*                                                         temp window + all windows */
  const uint32_t controllers_size = 2 * (sizeof(wndpool_t) + window_size + windows_size);
  mc_msg_t* const this = malloc(sizeof(mc_msg_t) + controllers_size);// TODO(MN): Remove as soon as possible

  this->read             = read_fn;
  this->write            = write_fn;
  this->on_receive       = on_receive;

  this->rcv              = (wndpool_t*)((char*)this + sizeof(mc_msg_t));
  this->rcv->window_size = window_size;
  this->rcv->data_size   = window_size - sizeof(pkt_t);
  this->rcv->capacity    = capacity;
  this->rcv->windows     = (wnd_t*)((char*)this->rcv->temp_window + window_size);

  this->snd              = (wndpool_t*)(char*)(this->rcv->windows) + windows_size;
  this->snd->window_size = window_size;
  this->snd->data_size   = window_size - sizeof(pkt_t);
  this->snd->capacity    = capacity;
  this->snd->windows     = (wnd_t*)((char*)this->snd->temp_window + window_size);

  wndpool_clear(this->rcv);
  wndpool_clear(this->snd);
  
  return this;
}

void mc_msg_free(mc_msg_t** const this)
{
  // TODO(MN): Check inputs
  free(*this);
  *this = NULL;
}

uint32_t mc_msg_recv(mc_msg_t* const this)
{
  const uint32_t size = read_data(this);
  send_unacked(this);
  return size;
}

uint32_t mc_msg_send(mc_msg_t* const this, void* data, uint32_t size)
{
  // TODO(MN): if size > this->window_size
  mc_msg_recv(this);

  if (wndpool_is_full(this->snd)) {
    return 0; // TODO(MN): Error
  }
  
  const wnd_t* const window = wndpool_get(this->snd, this->snd->end_id);
  wndpool_push(this->snd, mc_span(data, size));
  this->write(&window->packet, this->snd->window_size); // TODO(MN): Handle incomplete sending. also handle a timeout if fails continuously
  
  return size;
}

bool mc_msg_flush(mc_msg_t* const this, uint32_t timeout_us)
{
  const mc_time_t bgn_time_us = mc_now_u();

  while (!wndpool_is_empty(this->snd) || !wndpool_is_empty(this->rcv)) {
    mc_msg_recv(this);

    if ((mc_now_u() - bgn_time_us) > timeout_us) {
      return false;
    }
  }

  return true;
}


#undef MAX_TIMEOUT_US
