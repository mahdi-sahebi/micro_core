/* TODO(MN): Check inputs
 * Remove dynamic allocation
 * Remove standard library dependencies as much as possible
 * Define a network interface with write and read handlers
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "core/error.h"
#include "io/message/message.h"


#define DATA_LEN          10

enum definitions
{
  INVALID_ID = -1,
  HEADER     = 0xA5B3C7E9
};

typedef enum {
  PKT_DATA = 0,
  PKT_ACK
}packet_type_t;

typedef struct {
  uint32_t      header;
  packet_type_t type;
  uint32_t      id;
  uint32_t      size;
  char          data[0];
}packet_t;// TODO(MN): As size as window_size

typedef struct {
  uint16_t send_count;
  bool     is_acked;
  packet_t packet;
}window_t;

typedef struct
{
  uint32_t  begin_window_id;
  uint32_t  next_window_id;
  uint32_t  begin_index;
  uint32_t  end_index;
  uint32_t  count;
  uint32_t  window_size;
  uint32_t  data_size;
  uint32_t  capacity;
  window_t* windows;
  char      temp_window[0];
}controller_t;

struct _mc_msg_t
{ 
  mc_msg_read_fn  read;
  mc_msg_write_fn write;
  mc_msg_on_receive_fn on_receive;
  mc_time_now_us_fn now_us;

  // Receive
  uint32_t rcv_last_id;

  controller_t* rcv;// TODO(MN): Use array to reduce one pointer size
  controller_t* snd;  
};



static inline window_t* get_window(const controller_t* const this, uint16_t index)
{
  return (window_t*)((char*)(this->windows) + (index * (sizeof(window_t) + this->data_size)));// TODO(MN): Rcv/snd
}

static void advance_end_window(controller_t* window)
{
  window->next_window_id++;
  window->end_index = (window->end_index + 1) % window->capacity;
  window->count++;
}

static void push_window(controller_t* controller, void* data, uint32_t size)
{
  window_t* const window = get_window(controller, controller->end_index);
  window->packet.header  = HEADER;
  window->packet.type    = PKT_DATA;
  window->is_acked       = false;
  window->packet.size    = size;
  window->packet.id      = controller->next_window_id;
  window->send_count     = 0;
  memcpy(window->packet.data, data, size);
}

static void remove_acked_windows(controller_t* controller)
{
  while (get_window(controller, controller->begin_index)->is_acked && (INVALID_ID != get_window(controller, controller->begin_index)->packet.id)) {
    window_t* const window = get_window(controller, controller->begin_index);
    window->send_count    = 0;
    window->packet.header = 0;
    window->packet.id     = -1;
    window->is_acked      = true;

    controller->begin_window_id++;// TODO(MN): Use advance window
    controller->begin_index = (controller->begin_index + 1) % controller->capacity;
    controller->count--;
  }
}

static void window_read_ack(controller_t* controller, uint32_t window_id)
 {
  if ((window_id < controller->begin_window_id) || (window_id >= controller->begin_window_id + controller->capacity)) {
    return;
  }
  
  const uint32_t window_index = ((window_id - controller->begin_window_id) + controller->begin_index) % controller->capacity;
  window_t* const window = get_window(controller, window_index);
  if (!window->is_acked) {
    window->is_acked = true;
    // [WINDOW %u-%u] Received ACK for packet %u - %uus\n",              this->begin_window_id, this->begin_window_id+this->capacity-1, window_id, TimeNowU() - window->last_sent_time_us));
  }
}

/////////////////////////////////////////////////// rcv

static void rcv_send_ack(mc_msg_t* const this, uint32_t id)
{
  // if ((id < this->begin_window_id) || (this->next_window_id < id)) {
  //     // [ERROR] Attempted to ACK invalid seq %u (MAX_SEQ=%d)\n", seq, MAX_SEQ;
  //     return;
  // }
  packet_t* const packet = (packet_t*)(this->snd->temp_window);

  packet->header = HEADER;
  packet->type   = PKT_ACK;
  packet->id     = id;
  
  const uint32_t size = this->write(packet, this->rcv->window_size);
  if (size != this->rcv->window_size) {
    // TODO(MN): Handle. Is it ok to 
  }
  // ("[PACKET %u] Sent ACK (Total ACKs sent: %u)\n",         seq, total_packets_received);
}

static uint32_t read_data(mc_msg_t* const this)
{
  packet_t* const pkt = (packet_t*)(this->rcv->temp_window);
  const uint32_t read_size = this->read(pkt, this->rcv->window_size);
  if (0 == read_size) {// TODO(MN): Handle incomplete size
    return 0;
  }
  // TODO(MN): If read_size is not equal to this->window_size

  if (HEADER != pkt->header) {// TODO(MN): Find header
      return 0; // [INVALID] Bad header/type received
  }

  if (-1 != this->rcv_last_id) {// TODO(MN): Handle invalid id on overflows(long time)
    if (pkt->id <= this->rcv_last_id) {// TODO(MN): Handle overflow
      rcv_send_ack(this, pkt->id);
      return 0;
    }

    const int dif =(pkt->id - this->rcv_last_id);
    if (dif > 1) {
      // printf("f\n");
      return 0;
    }
  }
  this->rcv_last_id = pkt->id;

  // TODO(MN): Handle if header not valid. search for header to lock.
  // TODO(MN): Handle if read_size is not equal to a packet.
  if (PKT_ACK == pkt->type) {
    window_read_ack(this->snd, pkt->id);

    if (pkt->id == this->snd->begin_window_id) {
      remove_acked_windows(this->snd);
    }
  } else {
    rcv_send_ack(this, pkt->id);
    this->on_receive(pkt->data, pkt->size);
  }

  return read_size;
}

/////////////////////////////////////////////////// snd
static uint32_t snd_write_window(mc_msg_t* const this, uint32_t window_index) 
{
  window_t* const window = get_window(this->snd, window_index);
  const uint32_t sent_size = this->write(&window->packet, this->snd->window_size);

  if (0 != sent_size) {
    window->send_count++;
  }

  return sent_size;
}

static uint32_t snd_send_unacked(mc_msg_t* const this) 
{
  uint32_t sent_size = 0;
  
  for (uint32_t window_index = 0; window_index < this->snd->capacity; window_index++) {
    if (INVALID_ID == get_window(this->snd, window_index)->packet.id) {
      continue;
    }
    
    read_data(this);
    window_t* const window = get_window(this->snd, window_index);
    if (window->is_acked) {// TODO(MN): Check timeout occurance
        continue;
    }

    sent_size += window->packet.size;
    
    if (0 != snd_write_window(this, window_index)) {
    }// TODO(MN): Handle if send is incomplete. attempt 3 times! 
  }

  return sent_size;
}

/////////////////////////////////////////////////// message layer

mc_msg_t* mc_msg_new(
  mc_msg_read_fn read_fn, 
  mc_msg_write_fn write_fn, 
  uint32_t window_size, 
  uint32_t capacity, 
  mc_msg_on_receive_fn on_receive,
  mc_time_now_us_fn now_us)
{
  // TODO(MN): Input checking. the minimum size of window_size
  if ((NULL == read_fn) || (NULL == write_fn) || (NULL == now_us) || 
      (0 == window_size) || (0 == capacity)) {
    return NULL;// TODO(MN): MC_ERR_INVALID_ARGUMENT;
  }

  if (window_size < (sizeof(packet_t) + 1)) {
    return NULL;//MC_ERR_MEMORY_OUT_OF_RANGE;
  }
  
  const uint32_t windows_size = capacity * (sizeof(window_t) + window_size);
  const uint32_t controllers_size = 2 * (sizeof(controller_t) + window_size + windows_size);
  mc_msg_t* const this = malloc(sizeof(mc_msg_t) + controllers_size);

  this->read             = read_fn;
  this->write            = write_fn;
  this->on_receive       = on_receive;
  this->now_us           = now_us;

  this->rcv              = (controller_t*)((char*)this + sizeof(mc_msg_t));
  this->rcv->window_size = window_size;
  this->rcv->data_size   = window_size - sizeof(packet_t);
  this->rcv->capacity    = capacity;
  this->rcv->windows     = (window_t*)(this->rcv->temp_window + window_size);

  this->snd              = (controller_t*)(char*)(this->rcv->windows) + windows_size;
  this->snd->window_size = window_size;
  this->snd->data_size   = window_size - sizeof(packet_t);
  this->snd->capacity    = capacity;
  this->snd->windows     = (window_t*)(this->snd->temp_window + window_size);

  mc_msg_clear(this);

  for (uint32_t index = 0; index < capacity; index++) {
    get_window(this->rcv, index)->packet.id = INVALID_ID;
    get_window(this->snd, index)->packet.id = INVALID_ID;
  }
  
  return this;
}

void mc_msg_free(mc_msg_t** const this)
{
  // TODO(MN): Check inputs
  free(*this);
  *this = NULL;
}

mc_result mc_msg_clear(mc_msg_t* const this)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }

  this->rcv_last_id = -1;// TOOD(MN): Move to rcv controller

  this->rcv->begin_window_id = 0;
  this->rcv->next_window_id  = 0;
  this->rcv->begin_index     = 0;
  this->rcv->end_index       = 0;
  this->rcv->count           = 0;

  this->snd->begin_window_id = 0;
  this->snd->next_window_id  = 0;
  this->snd->begin_index     = 0;
  this->snd->end_index       = 0;
  this->snd->count           = 0;

  return MC_SUCCESS;
}

uint32_t mc_msg_read(mc_msg_t* const this)
{
  const uint32_t size = read_data(this);
  snd_send_unacked(this);
  return size;
}

bool mc_msg_read_finish(mc_msg_t* const this, uint32_t timeout_us)
{
  const uint32_t bgn_time_us = this->now_us();

  while (this->rcv->count) {
    mc_msg_read(this);

    if ((this->now_us() - bgn_time_us) > timeout_us) {
      return false;
    }
  }

  return true;
}

uint32_t mc_msg_write(mc_msg_t* const this, void* data, uint32_t size)
{
  // if size > this->window_size
  mc_msg_read(this);
  // TODO(MN): Check is not full
  if (mc_msg_is_full(this)) {
    return 0; // Error
  }

  push_window(this->snd, data, size);
  
  uint32_t sent_size = 0;
  do {
    sent_size = snd_write_window(this, this->snd->end_index);
  } while (0 == sent_size);// TODO(MN): Handle incomplete sending. also handle a timeout if fails continuously
  
  advance_end_window(this->snd);

  return size;//this->windows->packet.size;
}

bool mc_msg_write_finish(mc_msg_t* const this, uint32_t timeout_us)
{
  const uint32_t bgn_time_us = this->now_us();

  while (this->snd->count) {
    mc_msg_read(this);

    if ((this->now_us() - bgn_time_us) > timeout_us) {
      return false;
    }
  }

  return true;
}

uint32_t mc_msg_get_capacity(mc_msg_t* const this)
{
  return this->snd->capacity;// TODO(MN): Snd or rcv
}

uint32_t mc_msg_get_count(mc_msg_t* const this)
{
  return this->snd->count;// TODO(MN): Snd or rcv
}

uint32_t  mc_msg_get_window_size(mc_msg_t* const this)
{
  return this->snd->window_size;// TODO(MN): Test
}

bool mc_msg_is_empty(mc_msg_t* const this)
{
  return (0 == this->snd->count);// TODO(MN): Rcv or snd?
}

bool mc_msg_is_full(mc_msg_t* const this)// TODO(MN): snd_is_full
{
  return (this->snd->count == this->snd->capacity);// TODO(MN): Rcv or snd?
}


#undef DATA_LEN
