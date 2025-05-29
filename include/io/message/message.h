#ifndef MC_IO_MESSAGE_H_
#define MC_IO_MESSAGE_H_

#include <stdint.h>

typedef struct _mc_msg_t mc_msg_t;
typedef uint32_t (*mc_msg_read_fn)(void* const data, uint32_t size);
typedef uint32_t (*mc_msg_write_fn)(const void* const data, uint32_t size);
typedef void     (*mc_msg_on_receive_fn)(const void* const data, uint32_t size);


mc_msg_t* mc_msg_new(mc_msg_read_fn read_fn, mc_msg_write_fn write_fn, uint32_t window_size, uint32_t window_capacity, mc_msg_on_receive_fn on_receive);
void      mc_msg_free(mc_msg_t** const msg);

#endif /* MC_IO_MESSAGE_H_ */
