#include "io/io.h"
#include "io/communication/window_pool.h"// TODO(MN): Make it private
#include "mc_transceiver.h"


mc_chain_data mc_transceiver_send(mc_span buffer, void* arg)
{
  const mc_io* io = arg;
  const uint32_t sent_size = io->send(buffer.data, buffer.capacity);
  return mc_chain_data(mc_span(buffer.data, sent_size), MC_SUCCESS);
}

mc_chain_data mc_transceiver_recv(mc_span buffer, void* arg)
{
  const mc_io* io = arg;
  const uint32_t read_size = io->recv(buffer.data, buffer.capacity);
  return mc_chain_data(mc_span(buffer.data, read_size), MC_SUCCESS);
}
