#include <stdlib.h>
#include <string.h>
#include "mc_window_pool.h"


#define MIN(A, B)           ((A) <= (B) ? (A) : (B))


static inline mc_wnd_idx get_index(const wndpool_t* this, const mc_pkt_id id)
{
  int16_t dif = id - this->bgn_id;
  if (id < this->bgn_id) {
    dif += this->capacity;
  }

  return (this->bgn_id + dif) % this->capacity;
}

static inline wnd_t* get_window(const wndpool_t* this, const mc_wnd_idx index)
{
  return (wnd_t*)((char*)(this->windows) + (index * wnd_get_size(this->window_size)));// TODO(MN): Rcv/snd
}

static inline bool is_first_acked(const wndpool_t* this)
{
  const wnd_t* const window = get_window(this, this->bgn_index);
  return wnd_is_valid(window) && wnd_is_acked(window);
}

static void remove_first(wndpool_t* this)
{
  wnd_clear(get_window(this, this->bgn_index));

  this->bgn_id++;
  this->bgn_index = (this->bgn_index + 1) % this->capacity;

  if (this->end_id < this->bgn_id) {
    this->end_id = this->bgn_id;
  }
}

static void remove_acked(wndpool_t* this)
{
  while (is_first_acked(this)) {
    wnd_t* const window = get_window(this, this->bgn_index);
    wnd_clear(window);
    remove_first(this);
  }
}

void wndpool_init(wndpool_t* this, uint16_t window_size, uint8_t capacity)
{
  this->window_size = window_size;
  this->capacity    = capacity;
  wndpool_clear(this);
}

void wndpool_clear(wndpool_t* this)
{
  // TODO(MN): Ignore this loop. separate data and meta list to exploit memcpy 
  mc_wnd_idx index = this->capacity;
  while (index--) {
    wnd_clear(get_window(this, index));
  }

  this->bgn_id    = 0;
  this->end_id    = 0;
  this->bgn_index = 0;
}

bool wndpool_contains(const wndpool_t* this, mc_pkt_id id)
{
  return ((this->bgn_id <= id) && (id < this->bgn_id + this->capacity));
}

wnd_t* wndpool_get(wndpool_t* this, mc_pkt_id id)
{
  return get_window(this, get_index(this, id));
}

wnd_t* wndpool_get_last(wndpool_t* this)
{
  return wndpool_get(this, this->end_id);
}

bool wndpool_update(wndpool_t* this, mc_buffer buffer, mc_pkt_id id)
{
  if (!wndpool_contains(this, id)) {
    return false;
  }
  
  const mc_wnd_idx index = get_index(this, id);
  wnd_t* const window = get_window(this, index);
  window->packet.size = mc_buffer_get_size(buffer) - sizeof(window->packet);
  memcpy(&window->packet, buffer.data, mc_buffer_get_size(buffer));
  window->is_acked = true;

  return true;
}

uint8_t wndpool_get_count(wndpool_t* this)
{
  wnd_t* const window = wndpool_get_last(this);
  const uint8_t incomplete = 0 != window->packet.size;
  return (this->end_id - this->bgn_id) + incomplete;
}

bool wndpool_is_empty(wndpool_t* this)
{
  if (this->end_id == this->bgn_id) {
    return (0 == wndpool_get_last(this)->packet.size);
  }

  return false;
}

void wndpool_update_header(wndpool_t* this)
{
  wnd_t* const window = wndpool_get_last(this);// TODO(MN): [PR2]: Pass window, instead of get window

  window->packet.header = HEADER;
  window->packet.type   = PKT_DATA;
  window->packet.id     = this->end_id;
  window->sent_time_us  = mc_now_u();
  window->packet.crc    = 0x0000;
  window->packet.crc    = mc_alg_crc16_ccitt(mc_buffer(&window->packet, this->window_size)).value;
  window->is_acked      = false;
  window->is_sent       = true;
}

bool wndpool_ack(wndpool_t* this, mc_pkt_id id)
{
  cuint32_t window_index = get_index(this, id);
  wnd_t* const window = get_window(this, window_index);
  if (!wnd_is_acked(window)) {
    wnd_ack(window);
  }

  if (id == this->bgn_id) {
    remove_acked(this);
  }
  
  return true;
}

uint32_t wndpool_read(wndpool_t* this, mc_buffer buffer)
{
  if (!is_first_acked(this)) {
    return 0;
  }

  // TODO(MN): reading less than window size caues window drop!
  // Store the last read bytes. requires the continuous data pools
  // Separate the wnd(s) meta data and data buffers
  wnd_t* const window = wndpool_get(this, this->bgn_id);
  cuint32_t read_size = MIN(wnd_get_data_size(window) - this->stored_size, buffer.capacity);
  memcpy(buffer.data, wnd_get_data(window) + this->stored_size, read_size);

  this->stored_size += read_size;

  if (this->stored_size >= wnd_get_data_size(window)) {
    this->stored_size = 0;
    remove_first(this);
  }
  
  return read_size;
}

uint32_t wndpool_write(wndpool_t* this, mc_buffer buffer, wndpool_cb_done on_done, void* arg)
{
  if ((wndpool_get_count(this) == this->capacity) ||
      (0 == buffer.capacity) ||
      wndpool_get_last(this)->is_sent){
    return 0;// TODO(MN): Requires always one window be free. solve it
  }

  uint32_t data_size = buffer.capacity;
  uint32_t sent_size = 0;

  while (data_size) {
    wnd_t* const window = wndpool_get_last(this);// TODO(MN): Use index
    if (window->is_sent) {
      return 0;
    }  

    cuint32_t available_size = wnd_get_payload_size(this->window_size) - window->packet.size;
    cuint32_t seg_size = MIN(data_size, available_size);
    memcpy(window->packet.data + window->packet.size, buffer.data + sent_size, seg_size);
    
    window->packet.size += seg_size;
    this->update_time = mc_now_m();
    data_size -= seg_size;
    sent_size += seg_size;

    if (window->packet.size == wnd_get_payload_size(this->window_size)) {
      const mc_buffer window_buffer = mc_buffer(&window->packet, this->window_size);

      // TODO(MN): Optimize it.
      if (wndpool_get_count(this) < this->capacity) {
        wndpool_update_header(this);// TODO(MN): Get window - opt
        this->end_id++;// TODO(MN): Handle overflow. Add tests for long-term
        wnd_clear(wndpool_get_last(this));
      }

      if (NULL != on_done) {
        on_done(window_buffer, arg);
      }
    }
  }

  return sent_size;
}

bool wndpool_has_incomplete(wndpool_t* this)
{
  if (!wndpool_is_empty(this)) {
    const wnd_t* const window = wndpool_get_last(this);// TODO(MN): [PR2]: Pass window, instead of get window
    if ((0 < window->packet.size) && (window->packet.size != wnd_get_payload_size(this->window_size))) {
      return true;
    }
  }

  return false;
}


#undef MIN