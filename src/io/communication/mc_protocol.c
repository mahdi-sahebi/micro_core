#include "mc_base.h"
#include "mc_io.h"
#include "mc_frame.h"
#include "mc_protocol.h"


enum
{
  FLUSH_TIMEOUT_MS = 250,
};

static void send_ack(mc_comm* this, uint32_t id)
{
  mc_pkt* const pkt = this->snd->temp_window;

  pkt->header = HEADER;
  pkt->type   = PKT_ACK;
  pkt->id     = id;
  pkt->crc    = 0x0000;
  pkt->crc    = mc_alg_crc16_ccitt(mc_buffer(pkt, this->snd->pool.window_size)).value;

  io_send(this, pkt, this->snd->pool.window_size);
}

static void on_send_window_ready(const mc_buffer buffer, void* arg)
{
  mc_comm* this = arg;
  io_send(this, buffer.data, buffer.capacity);
}

static void send_stale_incomplete(mc_comm* const this)
{
#define WNDPOOL    this->snd->pool
  if (wndpool_has_incomplete(&WNDPOOL)) {
    if (mc_now_m() > (WNDPOOL.update_time_ms + FLUSH_TIMEOUT_MS)) {// TODO(MN): Update time is extra?
      wnd_t* const window = wndpool_get_last(&WNDPOOL);// TODO(MN): [PR2]: Pass window, instead of get window. Called twice
      if (!window->is_sent) {
        wndpool_update_header(&WNDPOOL);
      }

      io_send(this, &window->packet, WNDPOOL.window_size);
    }
  }
#undef WNDPOOL
}

void protocol_init(mc_comm* this)
{
  this->send_delay_us = MIN_SEND_TIME_US;
}

void protocol_recv(const mc_buffer buffer, void* arg)
{
  mc_comm* this = (mc_comm*)arg;
  mc_pkt* const pkt = (mc_pkt*)buffer.data;

  if (PKT_ACK == pkt->type) {
    if (!wndpool_contains(&this->snd->pool, pkt->id)) {// TODO(MN): Test that not read to send ack to let sender sends more
      return;
    }
    
    // TODO(MN): Not per ack
    cuint64_t elapsed_time = mc_now_u() - wndpool_get(&this->snd->pool, pkt->id)->sent_time_us;
    this->send_delay_us = MIN(MAX(elapsed_time * 0.8, MIN_SEND_TIME_US), MAX_SEND_TIME_US);
    wndpool_ack(&this->snd->pool, pkt->id);
    return;
  }

  if (pkt->id < this->rcv->pool.bgn_id) {// TODO(MN): Handle overflow
    send_ack(this, pkt->id);
    return;// done
  }

  if (wndpool_contains(&this->rcv->pool, pkt->id)) {
    wndpool_update(&this->rcv->pool, buffer, pkt->id);
    send_ack(this, pkt->id);
  }
}

mc_buffer protocol_send(mc_comm* this, mc_buffer buffer)
{
  buffer = frame_send(this->snd, buffer, on_send_window_ready, this);
  return buffer;
}

void protocol_send_unacked(mc_comm* const this) 
{
  const mc_time_t now = mc_now_u();

  for (mc_pkt_id id = this->snd->pool.bgn_id; id < this->snd->pool.end_id; id++) {
    wnd_t* const window = wndpool_get(&this->snd->pool, id);// TODO(MN): Error: first is acked but not removed
    if (wnd_is_acked(window) || (now < (window->sent_time_us + this->send_delay_us))) {// TODO(MN): Find the cause of unremoved first acked
      continue;
    }

    if (io_send(this, &window->packet, this->snd->pool.window_size)) {
      // window->sent_time_us = mc_now_u(); // TODO(MN): Why makes slower?
    }
  }

  send_stale_incomplete(this);
}

#undef AUTO_FLUSH_TIMEOUT_MS
