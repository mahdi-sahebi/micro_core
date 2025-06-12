#include <stdlib.h>
#include "io/message/window_pool.h"


static inline wnd_t* get_window(const wndpool_t* const this, uint16_t index)
{
  return (wnd_t*)((char*)(this->windows) + (index * (sizeof(wnd_t) + this->data_size)));// TODO(MN): Rcv/snd
}

static void advance_end_window(wndpool_t* this)
{
  this->end_id++;
  this->end_index = (this->end_index + 1) % this->capacity;
  this->count++;
}

void wndpool_clear(wndpool_t* const this)
{
  // TODO(MN): Ignore this loop
  for (uint32_t index = 0; index < this->capacity; index++) {
    wnd_clear(get_window(this, index));
  }

  this->bgn_id = 0;
  this->end_id  = 0;
  this->bgn_index     = 0;
  this->end_index       = 0;
  this->count           = 0;
}

bool wndpool_is_empty(wndpool_t* const this)
{
  return false;
}

bool wndpool_is_full(wndpool_t* const this)
{
  return false;
}

bool wndpool_contains(wndpool_t* const this, id_t id)
{
  return false;
}

wnd_t* wndpool_get(wndpool_t* const this, id_t id)
{
  return NULL;
}

bool wndpool_enqueue(wndpool_t* const this, const mc_span data)
{
  wnd_t* const window = get_window(this, this->end_index);
  wnd_write(window, data, this->end_id);
  return true;
}

bool wndpool_dequeue(wndpool_t* const this, const mc_span data)
{
  return false;
}

uint32_t wndpool_get_capacity(wndpool_t* const this)
{
  return 0;
}

id_t wndpool_get_bgn_id(wndpool_t* const this)
{
  return this->bgn_id;
}

id_t wndpool_get_end_id(wndpool_t* const this)
{
  return this->bgn_id + this->capacity;
}

bool wndpool_push(wndpool_t* const this, const mc_span data)
{
  wnd_t* const window = get_window(this, this->end_index);
  wnd_write(window, data, this->end_id);
  return true;
}

bool wndpool_ack(wndpool_t* const this, id_t id, wndpool_on_done_fn on_done)
{
  if ((id < this->bgn_id) || (id >= this->bgn_id + this->capacity)) {
    return false;
  }
  
  // TODO(MN): on_done if id==bgn_id
  if (id == this->bgn_id) {
    
  }


  const uint32_t window_index = ((id - this->bgn_id) + this->bgn_index) % this->capacity;
  wnd_t* const window = get_window(this, window_index);
  if (!wnd_is_acked(window)) {
    wnd_ack(window);
    // [WINDOW %u-%u] Received ACK for packet %u - %uus\n",              this->bgn_id, this->bgn_id+this->capacity-1, window_id, TimeNowU() - window->last_sent_time_us));
  }

  return true;
}
