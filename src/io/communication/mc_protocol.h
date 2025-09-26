#ifndef MC_IO_COMMUNICATION_PROTOCOL_H_
#define MC_IO_COMMUNICATION_PROTOCOL_H_

#include "alg/mc_buffer.h"


void      protocol_init();
void      protocol_recv(const mc_buffer buffer, void* arg);
mc_buffer protocol_send(mc_comm* this, mc_buffer buffer);
void      protocol_send_unacked(mc_comm* const this) ;


#endif /* MC_IO_COMMUNICATION_PROTOCOL_H_ */
