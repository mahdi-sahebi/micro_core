#ifndef MC_IO_COMMUNICATION_TRANSCEIVER_H_
#define MC_IO_COMMUNICATION_TRANSCEIVER_H_

#include "pattern/mc_chain.h"

mc_chain_data mc_transceiver_send(mc_span buffer, void* arg);
mc_chain_data mc_transceiver_recv(mc_span buffer, void* arg);

#endif /* MC_IO_COMMUNICATION_TRANSCEIVER_H_ */
