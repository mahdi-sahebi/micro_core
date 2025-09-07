#include <stdlib.h>
#include <string.h>
#include "io/communication/window_pool.h"


#define MIN(A, B)           ((A) <= (B) ? (A) : (B))


static mc_wnd_idx get_index(const wndpool_t* this, const mc_pkt_id id)
{
  int16_t dif = id - this->bgn_id;
  if (id < this->bgn_id) {
    dif += this->capacity;
  }

  return (this->bgn_id + dif) % this->capacity;
}

static wnd_t* get_window(const wndpool_t* this, const mc_wnd_idx index)
{
  // TODO(MN): Optimize
  return (wnd_t*)((char*)(this->windows) + (index * wnd_get_size(this->window_size)));// TODO(MN): Rcv/snd
}

static bool is_first_acked(const wndpool_t* this)
{
  const wnd_t* window = get_window(this, this->bgn_index);
  return wnd_is_valid(window) && wnd_is_acked(window);
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

bool wndpool_contains(wndpool_t* this, mc_pkt_id id)
{
  return ((this->bgn_id <= id) && (id < this->bgn_id + this->capacity));
}

wnd_t* wndpool_get(wndpool_t* this, mc_pkt_id id)
{
  return get_window(this, get_index(this, id));
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

bool wndpool_update(wndpool_t* this, mc_buffer buffer, mc_pkt_id id)
{
  if (!wndpool_contains(this, id)) {
    return false;
  }
  
  const mc_wnd_idx index = get_index(this, id);
  wnd_t* const window = get_window(this, index);
  wnd_write(window, buffer, id);
  window->packet.size = mc_buffer_get_size(buffer);
  memcpy(window->packet.data, buffer.data, mc_buffer_get_size(buffer));
  window->packet.crc = 0x0000;// TODO(MN): Not necessary here
  window->packet.crc = mc_alg_crc16_ccitt(mc_buffer(&window->packet, this->window_size)).value;
  window->is_acked = true;

  return true;
}

uint32_t wndpool_pop(wndpool_t* this, void* buffer, uint32_t size)
{
  if (!is_first_acked(this)) {
    return 0;
  }

  // TODO(MN): reading less than window size caues window drop!
  // Store the last read bytes. requires the continuous data pools
  // Separate the wnd(s) meta data and data buffers
  wnd_t* const window = wndpool_get(this, this->bgn_id);
  const uint32_t read_size = MIN(wnd_get_data_size(window) - this->itr_index, size);
  memcpy(buffer, wnd_get_data(window) + this->itr_index, read_size);

  this->itr_index += read_size;
  if (this->itr_index >= wnd_get_data_size(window)) {
    this->itr_index = 0;
    remove_first(this);
  }
  
  return read_size;
}

uint8_t wndpool_get_count(const wndpool_t* this)
{
  return (this->end_id - this->bgn_id);
}

bool wndpool_is_empty(const wndpool_t* this)
{
  if (this->end_id == this->bgn_id) {
    wnd_t* window = wndpool_get(this, this->end_id);
    return (0 == window->packet.size);
  }

  return false;
}

uint8_t wndpool_get_capacity(const wndpool_t* this)
{
  return this->capacity;
}

void wndpool_update_header(wndpool_t* this)
{
  wnd_t* const window = wndpool_get(this, this->end_id);// TODO(MN): Use index
  wnd_write(window, buffer, this->end_id);
  window->packet.crc = 0x0000;
  window->packet.crc = mc_alg_crc16_ccitt(mc_buffer(&window->packet, this->window_size)).value;
}

bool wndpool_push(wndpool_t* this, mc_buffer buffer)
{
  if (wndpool_get_count(this) == this->capacity) {
    return false; // TODO(MN): Error
  }

  wndpool_update_header(this);
  this->end_id++;

  // TODO(MN): Optimize it.
  wnd_clear(wndpool_get(this, this->end_id));

  return true;
}

bool wndpool_ack(wndpool_t* this, mc_pkt_id id)
{  
  const uint32_t window_index = get_index(this, id);
  wnd_t* const window = get_window(this, window_index);
  if (!wnd_is_acked(window)) {
    wnd_ack(window);
    // [WINDOW %u-%u] Received ACK for packet %u - %uus\n",              this->bgn_id, this->bgn_id+this->capacity-1, window_id, TimeNowU() - window->last_sent_time_us));
  }

  if (id == this->bgn_id) {
    remove_acked(this);
  }
  
  return true;
}

uint32_t wndpool_read(wndpool_t* this, mc_buffer buffer)
{
  return wndpool_pop(this, buffer.data, mc_buffer_get_size(buffer));
}

uint32_t wndpool_write(wndpool_t* this, mc_buffer buffer, wndpool_on_done_fn on_done, void* arg)
{
  if (wndpool_get_count(this) == this->capacity) {
    return 0;
  }

  uint32_t data_size = mc_buffer_get_size(buffer);
  uint32_t sent_size = 0;

  while (data_size) {
    wnd_t* const window = wndpool_get(this, this->end_id);// TODO(MN): Use index
    const uint32_t available_size = wnd_get_payload_size(this->window_size) - window->packet.size;
    const uint32_t seg_size = MIN(data_size, available_size);
    memcpy(window->packet.data + window->packet.size, buffer.data + sent_size, seg_size);
    
    window->packet.size += seg_size;
    this->update_time = mc_now_m();
    data_size -= seg_size;
    sent_size += seg_size;

    if (window->packet.size == wnd_get_payload_size(this->window_size)) {
      const mc_buffer window_buffer = mc_buffer(&window->packet, this->window_size);
      const bool pushed = wndpool_push(this, window_buffer);

      if (NULL != on_done) {
        on_done(window_buffer, arg);
      }

      if (!pushed) {
        printf("sss\n");
        break;
      }
    }
  }

  return sent_size;
}



#undef MIN