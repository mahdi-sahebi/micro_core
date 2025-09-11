#include "mc_frame.h"



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

void frame_init(mc_frame* this, uint16_t window_size, uint8_t capacity)
{
  wndpool_init(&this->pool, window_size, capacity);
  this->temp_stored = 0;
  this->temp_window = (mc_pkt*)((char*)this->pool.windows + WNDPOOL_GET_WINDOWS_SIZE(window_size, capacity));
}

void frame_recv(mc_frame* this, mc_data_ready_cb data_ready, void* arg)
{
  if (!frame_is_completed(this)) {
    return;
  }

  this->temp_stored = 0;
  mc_pkt* const pkt = (mc_pkt*)this->temp_window;

  if (!frame_is_header_valid(pkt)) {// TODO(MN): Packet unlocked. Find header. simulated in tests to unlock
    frame_drop(pkt);
    return;
  }
  if (!frame_is_crc_valid(&this->pool, pkt)) {// Data corruption
    return;
  }

  // TODO(MN): Callback
  if (NULL != data_ready) {
    data_ready(mc_buffer(this->temp_window, this->pool.window_size), arg);
  }
}

mc_buffer frame_send(mc_frame* this, mc_buffer buffer, mc_data_ready_cb data_ready, void* arg)
{
  const uint32_t size = wndpool_write(&this->pool, buffer, data_ready, arg);
  return mc_buffer(buffer.data, size);
}
