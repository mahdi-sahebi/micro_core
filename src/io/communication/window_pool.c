#include <stdlib.h>
#include <string.h>
#include "io/communication/window_pool.h"


#define MIN(A, B)           ((A) <= (B) ? (A) : (B))


static mc_wnd_idx get_index(const wndpool_t* const this, const mc_pkt_id id)
{
  int16_t dif = id - this->bgn_id;
  if (id < this->bgn_id) {
    dif += this->capacity;
  }

  return (this->bgn_id + dif) % this->capacity;
}

static wnd_t* get_window(const wndpool_t* const this, const mc_wnd_idx index)
{
  return (wnd_t*)((char*)(this->windows) + (index * (this->window_size + (sizeof(wnd_t) - sizeof(mc_pkt)))));// TODO(MN): Rcv/snd
}

static bool is_first_acked(const wndpool_t* const this)
{
  return wnd_is_acked(get_window(this, this->bgn_index)) &&
         (INVALID_ID != get_window(this, this->bgn_index)->packet.id);
}

void wndpool_clear(wndpool_t* const this)
{
  // TODO(MN): Ignore this loop
  for (mc_wnd_idx index = 0; index < this->capacity; index++) {
    wnd_clear(get_window(this, index));
  }

  this->bgn_id    = 0;
  this->end_id    = 0;
  this->bgn_index = 0;
}

bool wndpool_is_empty(wndpool_t* const this)
{
  return (0 == wndpool_get_count(this));
}

bool wndpool_is_full(wndpool_t* const this)
{
  return (wndpool_get_count(this) == this->capacity);
}

bool wndpool_contains(wndpool_t* const this, mc_pkt_id id)
{
  return ((this->bgn_id <= id) && (id < this->bgn_id + this->capacity));
}

wnd_t* wndpool_get(wndpool_t* const this, mc_pkt_id id)
{
  return get_window(this, get_index(this, id));
}

static void remove_first(wndpool_t* const this)
{
  wnd_clear(get_window(this, this->bgn_index));

  this->bgn_id++;
  this->bgn_index = (this->bgn_index + 1) % this->capacity;

  if (this->end_id < this->bgn_id) {
    this->end_id = this->bgn_id;
  }
}

static void remove_acked(wndpool_t* const this, mc_io_receive_cb on_receive)
{
  while (is_first_acked(this)) {
    wnd_t* const window = get_window(this, this->bgn_index);

    if (NULL != on_receive) {
      on_receive(wnd_get_data(window), window->packet.size);
    }

    wnd_clear(window);
    remove_first(this);
  }
}

bool wndpool_update(wndpool_t* const this, mc_span data, mc_pkt_id id)
{
  if (!wndpool_contains(this, id)) {
    return false;
  }
  
  const mc_wnd_idx index = get_index(this, id);
  wnd_t* const window = get_window(this, index);
  wnd_write(window, data, id);
  window->is_acked = true;

  return true;
}

uint32_t wndpool_pop(wndpool_t* const this, void* data, uint32_t size)
{
  if (!is_first_acked(this)) {
    return 0;
  }

  // TODO(MN): reading less than window size caues window drop!
  // Store the last read bytes. requires the continuous data pools
  // Separate the wnd(s) meta data and data buffers
  wnd_t* const window = wndpool_get(this, this->bgn_id);
  const uint32_t read_size = MIN(wnd_get_data_size(window) - this->last_read_size, size);
  memcpy(data, wnd_get_data(window) + this->last_read_size, read_size);

  this->last_read_size += read_size;
  if (this->last_read_size >= wnd_get_data_size(window)) {
    this->last_read_size = 0;
    remove_first(this);
  }
  
  return read_size;
}

uint8_t wndpool_get_count(const wndpool_t* const this)
{
  uint8_t count = 0;

  for (mc_wnd_idx index = 0; index < this->capacity; index++) {
    if (wnd_is_valid(get_window(this, index))) {
      count++;
    }
  }

  return count;
}

uint8_t wndpool_get_capacity(const wndpool_t* const this)
{
  return this->capacity;
}

bool wndpool_push(wndpool_t* const this, const mc_span data)
{
  if (wndpool_is_full(this)) {
    return false; // TODO(MN): Error
  }

  wnd_t* const window = wndpool_get(this, this->end_id);
  wnd_write(window, data, this->end_id);
  this->end_id++;

  return true;
}

bool wndpool_ack(wndpool_t* const this, mc_pkt_id id, mc_io_receive_cb on_done)
{
  if (!wndpool_contains(this, id)) {
    return false;
  }
  
  const uint32_t window_index = get_index(this, id);
  wnd_t* const window = get_window(this, window_index);
  if (!wnd_is_acked(window)) {
    wnd_ack(window);
    
    // [WINDOW %u-%u] Received ACK for packet %u - %uus\n",              this->bgn_id, this->bgn_id+this->capacity-1, window_id, TimeNowU() - window->last_sent_time_us));
  }

  if (id == this->bgn_id) {
    remove_acked(this, on_done);
  }
  
  return true;
}


#undef MIN