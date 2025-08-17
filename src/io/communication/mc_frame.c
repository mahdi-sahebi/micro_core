#include "mc_frame.h"
#include "io/communication/window_pool.h"


mc_chain_data mc_frame_send(mc_span buffer, void* arg)
{
  wndpool_t* const send_pool = (wndpool_t*)arg;

   const wnd_t* const window = wndpool_get(send_pool, send_pool->end_id);// TODO(MN): Bad design
  if (wndpool_push(send_pool, buffer)) { // TODO(MN): Don't Send incompleted windows, allow further sends attach their data
    return mc_chain_data(mc_span(&window->packet, send_pool->window_size), MC_SUCCESS);
  }

  return mc_chain_data_error(MC_ERR_OUT_OF_RANGE);// TODO(MN): Memroy not enough, 
}

mc_chain_data mc_frame_recv(mc_span buffer, void* arg)
{
  const wndpool_t* const recv_pool = (wndpool_t*)arg;

  // TODO(MN): Check all inputs
  if (buffer.capacity != recv_pool->window_size) {// TODO(MN): Full check
    return mc_chain_data_error(MC_ERR_INCOMPLETE);
    // return mc_chain_data(buffer, MC_SUCCESS);
  }

  mc_pkt* const pkt = (mc_pkt*)buffer.data;
  if (HEADER != pkt->header) {// TODO(MN): Packet unlocked. Find header
    return mc_chain_data_error(MC_ERR_RUNTIME); // [INVALID] Bad header/type received. 
  }

  const uint16_t received_crc = pkt->crc;
  pkt->crc = 0x0000;
  const uint16_t crc = mc_alg_crc16_ccitt(mc_span(pkt, recv_pool->window_size)).value;
  if (received_crc != crc) {
    return mc_chain_data_error(MC_ERR_RUNTIME);// TODO(MN): Data corruption
  }

  return mc_chain_data(buffer, MC_SUCCESS);
}
