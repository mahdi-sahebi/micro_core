#ifndef MC_IO_COMMUNICATION_FRAME_H_
#define MC_IO_COMMUNICATION_FRAME_H_

#include "alg/span.h"
#include "pattern/mc_chain.h"

mc_chain_data mc_frame_send(mc_span buffer, void* arg);
mc_chain_data mc_frame_recv(mc_span buffer, void* arg);

#endif /* MC_IO_COMMUNICATION_FRAME_H_ */
