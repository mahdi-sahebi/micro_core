#ifndef MC_IO_MESSAGE_H_
#define MC_IO_MESSAGE_H_

#include <stdint.h>
#include "core/error.h"
#include "io/io.h"

typedef struct _mc_msg_t mc_msg_t;


mc_msg_t* mc_msg_new(uint16_t window_size, uint8_t window_capacity, mc_io io, mc_io_receive_cb on_receive);
void      mc_msg_free(mc_msg_t** const msg);
uint32_t  mc_msg_recv(mc_msg_t* const msg);
uint32_t  mc_msg_send(mc_msg_t* const msg, void* data, uint32_t size);
bool      mc_msg_flush(mc_msg_t* const msg, uint32_t timeout_us);


#endif /* MC_IO_MESSAGE_H_ */
