#ifndef MC_IO_MESSAGE_H_
#define MC_IO_MESSAGE_H_

#include <stdint.h>

typedef struct _mc_msg_t mc_msg_t;
typedef uint32_t (*mc_msg_read_fn)(void* const data, uint32_t size);
typedef uint32_t (*mc_msg_write_fn)(const void* const data, uint32_t size);
typedef uint32_t (*mc_time_now_us_fn)();
typedef void     (*mc_msg_on_receive_fn)(const void* const data, uint32_t size);


mc_msg_t* mc_msg_new(mc_msg_read_fn read_fn, mc_msg_write_fn write_fn, uint32_t window_size, uint32_t capacity, mc_msg_on_receive_fn on_receive, mc_time_now_us_fn now_us);
void      mc_msg_free(mc_msg_t** const msg);
mc_result mc_msg_clear(mc_msg_t* const msg);
uint32_t  mc_msg_get_capacity(mc_msg_t* const msg);
uint32_t  mc_msg_get_count(mc_msg_t* const msg);
uint32_t  mc_msg_get_window_size(mc_msg_t* const msg);
bool      mc_msg_is_empty(mc_msg_t* const msg);
bool      mc_msg_is_full(mc_msg_t* const msg);
uint32_t  mc_msg_read(mc_msg_t* const msg);
uint32_t  mc_msg_write(mc_msg_t* const msg, void* data, uint32_t size);
bool      mc_msg_read_finish(mc_msg_t* const msg, uint32_t timeout_us);
bool      mc_msg_write_finish(mc_msg_t* const msg, uint32_t timeout_us);


#endif /* MC_IO_MESSAGE_H_ */
