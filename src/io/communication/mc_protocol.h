#ifndef MC_IO_COMMUNICATION_PROTOCOL_H_
#define MC_IO_COMMUNICATION_PROTOCOL_H_

#include "core/error.h"
#include "alg/span.h"
#include "io/io.h"
#include "pattern/mc_chain.h"


struct _mc_comm_t
{ 
  mc_io      io;
  uint32_t   send_delay_us;// TODO(MN): Use u16 with 100X us resolution
  wndpool_t* rcv;// TODO(MN): Use array to reduce one pointer size
  wndpool_t* snd;
  mc_chain*  recv_chain;// TODO(NM): Not related to the protocol layer
  mc_chain*  send_chain;
};

typedef struct _mc_comm_t mc_comm;


mc_chain_data mc_protocol_send(mc_span buffer, void* arg);
mc_chain_data mc_protocol_recv(mc_span buffer, void* arg);


#endif /* MC_IO_COMMUNICATION_PROTOCOL_H_ */
