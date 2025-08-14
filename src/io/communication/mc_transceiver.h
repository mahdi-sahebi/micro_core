#ifndef MC_IO_COMMUNICATION_TRANSCEIVER_H_
#define MC_IO_COMMUNICATION_TRANSCEIVER_H_

#include "pattern/mc_chain.h"

mc_chain_data mc_transceiver_send(mc_chain_data data);
mc_chain_data mc_transceiver_recv(mc_chain_data data);

#endif /* MC_IO_COMMUNICATION_TRANSCEIVER_H_ */
