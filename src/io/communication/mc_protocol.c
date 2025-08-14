// TODO(MN): Many nested if(s). use tiny functions for readability

#include "io/communication/window_pool.h"
#include "mc_protocol.h"


static bool send_buffer(mc_comm* this, const void* buffer, uint32_t size)
{
  uint8_t index = 5;
  while (index--) {
    if (size == this->io.send(buffer, size)) {
      return true;
    }// TODO(MN): Handle if send is incomplete. attempt 3 times! 
  }
  
  return false;
}

static void send_unacked(mc_comm* const this) 
{
  const mc_time_t now = mc_now_u();

  for (mc_pkt_id id = this->snd->bgn_id; id < this->snd->end_id; id++) {
    wnd_t* const window = wndpool_get(this->snd, id);
    if (wnd_is_acked(window) || (now < (window->sent_time_us + this->send_delay_us))) {
      continue;
    }

    if (send_buffer(this, &window->packet, this->snd->window_size)) {
      // window->sent_time_us = now;
    }
  }
}

static void send_ack(mc_comm* this, uint32_t id)
{
  mc_pkt* const pkt = this->snd->temp_window;

  pkt->header = HEADER;
  pkt->type   = PKT_ACK;
  pkt->id     = id;
  pkt->crc    = 0x0000;
  pkt->crc    = mc_alg_crc16_ccitt(mc_span(pkt, this->snd->window_size)).value;

  send_buffer(this, pkt, this->rcv->window_size);
}

mc_chain_data mc_protocol_send(mc_span buffer, void* arg)
{
  return mc_chain_data(buffer, MC_SUCCESS);
}

mc_chain_data mc_protocol_recv(mc_span buffer, void* arg)
{
  mc_comm* this = (mc_comm*)arg;
  if (buffer.capacity != this->rcv->window_size) {// TODO(MN): Full check
    return mc_chain_data_error(MC_ERR_INVALID_ARGUMENT);
  }

  mc_pkt* const pkt = (mc_pkt*)buffer.data;

  if (PKT_ACK == pkt->type) {
    if (!wndpool_contains(this->snd, pkt->id)) {// TODO(MN): Test that not read to send ack to let sender sends more
      return mc_chain_data_error(MC_ERR_RUNTIME);
    }

    // TODO(MN): Not per ack
    const mc_time_t sent_time_us = wndpool_get(this->snd, pkt->id)->sent_time_us;
    const uint64_t elapsed_time = mc_now_u() - sent_time_us;
    this->send_delay_us = elapsed_time * 0.8;
    wndpool_ack(this->snd, pkt->id);
    return mc_chain_data_error(MC_ERR_RUNTIME);// done
  }

  if (pkt->id < this->rcv->bgn_id) {// TODO(MN): Handle overflow
    send_ack(this, pkt->id);
    return mc_chain_data_error(MC_ERR_RUNTIME);// done
  }

  if (wndpool_update(this->rcv, mc_span(pkt->data, pkt->size), pkt->id)) {
    send_ack(this, pkt->id);
  }

  send_unacked(this);// TODO(MN): Call always. handle pre early returns
  return mc_chain_data(buffer, MC_SUCCESS);
}
