#ifndef MC_IO_MESSAGE_H_
#define MC_IO_MESSAGE_H_

#include <stdint.h>
#include "core/error.h"

typedef struct _mc_msg_t mc_msg_t;
typedef uint32_t (*mc_msg_read_fn)(void* const data, uint32_t size);
typedef uint32_t (*mc_msg_write_fn)(const void* const data, uint32_t size);
typedef uint32_t (*mc_time_now_us_fn)();
typedef void     (*mc_msg_on_receive_fn)(const void* const data, uint32_t size);


mc_msg_t* mc_msg_new(mc_msg_read_fn read_fn, mc_msg_write_fn write_fn, uint32_t window_size, uint8_t capacity, mc_msg_on_receive_fn on_receive, mc_time_now_us_fn now_us);
void      mc_msg_free(mc_msg_t** const msg);
uint32_t  mc_msg_recv(mc_msg_t* const msg);
uint32_t  mc_msg_send(mc_msg_t* const msg, void* data, uint32_t size);
bool      mc_msg_flush(mc_msg_t* const msg, uint32_t timeout_us);


#endif /* MC_IO_MESSAGE_H_ */
