#ifndef MC_IO_COMMUNICATION_IO_H_
#define MC_IO_COMMUNICATION_IO_H_

#include "io/io.h"
#include "alg/mc_buffer.h"
#include "mc_base.h"// TODO(MN): Delete

typedef void (*io_cb_data_ready)(const mc_buffer buffer, void* arg);


void io_init(mc_io* this, mc_io io);
void io_recv(mc_comm* this, io_cb_data_ready data_ready, void* arg);
bool io_send(mc_comm* this, cvoid* buffer, uint32_t size);


#endif /* MC_IO_COMMUNICATION_IO_H_ */
