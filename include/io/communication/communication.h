#ifndef MC_IO_MESSAGE_H_
#define MC_IO_MESSAGE_H_

#include <stdint.h>
#include "core/error.h"
#include "io/io.h"

typedef struct _mc_comm_t mc_comm_t;


mc_comm_t* mc_comm_new(uint16_t window_size, uint8_t window_capacity, mc_io io, mc_io_receive_cb on_receive);
void       mc_comm_free(mc_comm_t** const msg);
uint32_t   mc_comm_recv(mc_comm_t* const msg);
uint32_t   mc_comm_send(mc_comm_t* const msg, void* data, uint32_t size);
bool       mc_comm_flush(mc_comm_t* const msg, uint32_t timeout_us);


#endif /* MC_IO_MESSAGE_H_ */
