/* TODO(MN): Check inputs
 * Remove dynamic allocation
 * Remove standard library dependencies as much as possible
 * Define a network interface with write and read handlers
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "io/message/message.h"


#define DATA_LEN          10
#define WINDOW_LEN        3

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
  uint32_t      data[DATA_LEN];
}packet_t;

typedef struct {
  mc_msg_read_fn  read;
  mc_msg_write_fn write;
  packet_t packet;
  uint16_t send_count;
  bool     is_acked;
}window_t;

struct _mc_msg_t
{ 
  mc_msg_read_fn  read;
  mc_msg_write_fn write;
  mc_msg_on_receive_fn on_receive;
  window_t windows[WINDOW_LEN];
  uint32_t begin_window_id;
  uint32_t next_window_id;
  uint32_t begin_index;
  uint32_t end_index;
  uint32_t count;
  uint32_t capacity;
};



static uint32_t write_window(mc_msg_t* const msg, uint32_t window_index) 
{
  window_t* const window = &msg->windows[window_index];
  const uint32_t sent_size = msg->write(&window->packet, sizeof(window->packet));

  if (0 != sent_size) {
    window->send_count++;
  }

  return sent_size;
}

static void advance_end_window(mc_msg_t* const msg)
{
  msg->next_window_id++;
  msg->end_index = (msg->end_index + 1) % msg->capacity;
  msg->count++;
}

static void push_back(mc_msg_t* const msg, void* data, uint32_t size)
{
  window_t* const window = &msg->windows[msg->end_index];
  window->packet.header  = HEADER;
  window->packet.type    = PKT_DATA;
  window->is_acked       = false;
  window->packet.size    = size;
  window->packet.id      = msg->next_window_id;
  window->send_count     = 0;
  memcpy(window->packet.data, data, size);
}

static void clear_sending_acked_windows(mc_msg_t* const this)
{
  while (this->windows[this->begin_index].is_acked && (INVALID_ID != this->windows[this->begin_index].packet.id)) {
    window_t* const window = &this->windows[this->begin_index];
    window->send_count    = 0;
    window->packet.header = 0;
    window->packet.id     = -1;
    window->is_acked      = true;

    this->begin_window_id++;
    this->begin_index = (this->begin_index + 1) % this->capacity;
    this->count--;
  }
}

static void read_ack(mc_msg_t* const msg, uint32_t window_id)
 {
  if ((window_id < msg->begin_window_id) || (window_id >= msg->begin_window_id + msg->capacity)) {
    return;
  }
  
  const uint32_t window_index = ((window_id - msg->begin_window_id) + msg->begin_index) % msg->capacity;
  window_t* const window = &msg->windows[window_index];
  if (!window->is_acked) {
    window->is_acked = true;
    clear_acked_windows(msg);
    // [WINDOW %u-%u] Received ACK for packet %u - %uus\n",              msg->begin_window_id, msg->begin_window_id+msg->capacity-1, window_id, TimeNowU() - window->last_sent_time_us));
  }
}

static uint32_t read_data(mc_msg_t* const msg)
{
  packet_t pkt;// TODO(MN): Internal buffer
  const uint32_t read_size = msg->read(&pkt, sizeof(packet_t));
  if (0 == read_size) {
    return 0;
  }
  // TODO(MN): If read_size is not equal to sizeof(packet_t)

  if ((HEADER != pkt.header) || (PKT_DATA != pkt.type)) {
      return 0; // [INVALID] Bad header/type received
  }

  // TODO(MN): Handle if header not valid. search for header to lock.
  // TODO(MN): Handle if read_size is not equal to a packet.
  if ((HEADER == pkt.header) && (PKT_ACK == pkt.type)) {
    read_ack(msg, pkt.id);
  }

  return read_size;
}

static uint32_t send_unacked(mc_msg_t* const this) 
{
  uint32_t sent_size = 0;
  
  for (uint32_t window_index = 0; window_index < this->capacity; window_index++) {
    if (INVALID_ID == this->windows[window_index].packet.id) {
      continue;
    }
    
    read_data(this);
    window_t* const window = &this->windows[window_index];
    if (window->is_acked) {
        continue;
    }

    sent_size += sizeof(window->packet.data);
  }

  return sent_size;
}

mc_msg_t* mc_msg_new(mc_msg_read_fn read_fn, mc_msg_write_fn write_fn, uint32_t window_size, uint32_t window_capacity, mc_msg_on_receive_fn on_receive)
{
  mc_msg_t* msg = malloc(sizeof(mc_msg_t));

  msg->read            = read_fn;
  msg->write           = write_fn;
  msg->on_receive      = on_receive;
  msg->begin_window_id = 0;
  msg->next_window_id  = 0;
  msg->begin_index     = 0;
  msg->end_index       = 0;
  msg->count           = 0;
  msg->capacity        = WINDOW_LEN;

  for (uint32_t index = 0; index < msg->capacity; index++) {
    msg->windows[index].packet.id = INVALID_ID;
  }
  
  return msg;
}

void mc_msg_free(mc_msg_t** const msg)
{
  free(*msg);
  *msg = NULL;
}

void mc_msg_clear(mc_msg_t* const msg)
{

}

uint32_t mc_msg_read(mc_msg_t* const msg)
{
  const uint32_t size = read_data(msg);
  send_unacked(msg);
  return size;
}

uint32_t mc_msg_read_finish(mc_msg_t* const msg, uint32_t timeout_us)
{
  return 0;
}

uint32_t mc_msg_write(mc_msg_t* const msg, void* data, uint32_t size)
{
  // TODO(MN): Check is not full
  push_back(msg, data, size);
  write_window(msg, msg->end_index);
  advance_end_window(msg);

  return size;
}

uint32_t mc_msg_write_finish(mc_msg_t* const msg, uint32_t timeout_us)
{
  return 0;
}

uint32_t mc_msg_get_capacity(mc_msg_t* const msg)
{
  return msg->capacity;
}

uint32_t mc_msg_get_count(mc_msg_t* const msg)
{
  return msg->count;
}

bool mc_msg_is_empty(mc_msg_t* const msg)
{
  return (0 == msg->count);
}

bool mc_msg_is_full(mc_msg_t* const msg)
{
  return (msg->count == msg->capacity);
}


#undef WINDOW_LEN
#undef DATA_LEN
