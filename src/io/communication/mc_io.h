#ifndef MC_IO_COMMUNICATION_IO_H_
#define MC_IO_COMMUNICATION_IO_H_

#include "io/io.h"
#include "alg/mc_buffer.h"
#include "mc_base.h"// TODO(MN): Delete

typedef void (*io_data_ready_cb)(const mc_buffer buffer, void* arg);


void io_init(mc_io* this, mc_io io);
void io_recv(mc_comm* this, io_data_ready_cb data_ready, void* arg);
bool io_send(mc_comm* this, const void* buffer, uint32_t size);


#endif /* MC_IO_COMMUNICATION_IO_H_ */
