#include "io/io.h"
#include "mc_transceiver.h"


mc_chain_data mc_transceiver_send(mc_chain_data data)
{
  const mc_io* io = data.arg;
  const uint32_t sent_size = io->send(data.buffer.data, data.buffer.capacity);
  return mc_chain_data(NULL, mc_span(data.buffer.data, sent_size), MC_SUCCESS);
}

mc_chain_data mc_transceiver_recv(mc_chain_data data)
{
  const mc_io* io = data.arg;
  const uint32_t read_size = io->recv(data.buffer.data, data.buffer.capacity);
  return mc_chain_data(this, mc_span(data.buffer.data, read_size), MC_SUCCESS);
}
