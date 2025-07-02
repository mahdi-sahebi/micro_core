#include <stdlib.h>
#include "io/message/window_pool.h"


static uint32_t get_index(const wndpool_t* const this, uint32_t id)
{
  uint32_t dif = id - this->bgn_id;
  if (id < this->bgn_id) {
    dif += this->capacity;
  }

  return (this->bgn_id + dif) % this->capacity;
}

static inline wnd_t* get_window(const wndpool_t* const this, const uint32_t index)
{
  return (wnd_t*)((char*)(this->windows) + (index * (sizeof(wnd_t) + this->data_size)));// TODO(MN): Rcv/snd
}

uint32_t wndpool_get_count(wndpool_t* const this)
{
  uint32_t count = 0;

  for (uint32_t index = 0; index < this->capacity; index++) {
    if (wnd_is_valid(get_window(this, index))) {
      count++;
    }
  }

  return count;
}

static void data_receive(wnd_t* const window, uint32_t window_size, wndpool_on_done_fn on_done)
{
  if (NULL != on_done) {
    on_done(mc_span(wnd_get_data(window), window_size), window->packet.id);
  }
}

static bool is_first_acked(const wndpool_t* const this)
{
  return wnd_is_acked(get_window(this, this->bgn_index)) &&
         (INVALID_ID != get_window(this, this->bgn_index)->packet.id);
}

static void remove_acked_windows(wndpool_t* const this, wndpool_on_done_fn on_done)
{
  while (is_first_acked(this)) {
    wnd_t* const window = get_window(this, this->bgn_index);
    data_receive(window, this->window_size, on_done);
    wnd_clear(window);
    wndpool_remove_first(this);
  }
}

void wndpool_clear(wndpool_t* const this)
{
  // TODO(MN): Ignore this loop
  for (uint32_t index = 0; index < this->capacity; index++) {
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

bool wndpool_contains(wndpool_t* const this, id_t id)
{
  // TODO(MN): To be able to insert, we need to remove count and always have whole of 
  // pool. then define the contains() like below.
  return false;//((this->bgn_id <= id) && (id <= this->bgn_id + this->capacity));
}

wnd_t* wndpool_get(wndpool_t* const this, id_t id)
{
  return get_window(this, get_index(this, id));
}

bool wndpool_enqueue(wndpool_t* const this, const mc_span data)
{
  // wnd_t* const window = get_window(this, this->end_index);
  // wnd_write(window, data, this->end_id);
  // return true;
  return false;
}

bool wndpool_dequeue(wndpool_t* const this, const mc_span data)
{
  return false;
}

void wndpool_remove_first(wndpool_t* const this)
{
  wnd_clear(get_window(this, this->bgn_index));

  this->bgn_id++;
  this->bgn_index = (this->bgn_index + 1) % this->capacity;

  if (this->end_id < this->bgn_id) {
    this->end_id = this->bgn_id;
  }
}

bool wndpool_insert(wndpool_t* const this, const mc_span data, const id_t id)
{return false;
  if (!wndpool_contains(this, id)) {
    return false;
  }
  
  const uint32_t index = get_index(this, this->end_id);
  wnd_t* const window = get_window(this, index);
  wnd_write(window, data, this->end_id);
  this->end_id++;

  return true;
}

uint32_t wndpool_get_capacity(wndpool_t* const this)
{
  return this->capacity;
}

bool wndpool_push(wndpool_t* const this, const mc_span data)
{
  if (wndpool_is_full(this)) {
    return false;
  }
  
  wnd_t* const window = wndpool_get(this, this->end_id);
  wnd_write(window, data, this->end_id);
  this->end_id++;

  return true;
}

bool wndpool_ack(wndpool_t* const this, id_t id, wndpool_on_done_fn on_done)
{
  if ((id < this->bgn_id) || (id >= this->bgn_id + this->capacity)) {
    return false;
  }
  
  // // TODO(MN): on_done if id==bgn_id
  // if (id == this->bgn_id) {
    
  // }

  const uint32_t window_index = ((id - this->bgn_id) + this->bgn_index) % this->capacity;
  wnd_t* const window = get_window(this, window_index);
  if (!wnd_is_acked(window)) {
    wnd_ack(window);
    // [WINDOW %u-%u] Received ACK for packet %u - %uus\n",              this->bgn_id, this->bgn_id+this->capacity-1, window_id, TimeNowU() - window->last_sent_time_us));
  }

  if (id == this->bgn_id) {
    remove_acked_windows(this, on_done);
  }
  
  return true;
}
