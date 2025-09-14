#ifndef MC_IO_MESSAGE_H_
#define MC_IO_MESSAGE_H_

#include <stdint.h>
#include "core/error.h"
#include "alg/mc_buffer.h"
#include "io/io.h"

typedef struct _mc_msg mc_msg;
typedef uint16_t mc_msg_id;
typedef void (*mc_msg_receive_cb)(mc_msg_id, mc_buffer);

typedef struct
{
  mc_io    io;
  uint32_t recv_pool_size;
  uint16_t window_size;
  uint16_t ids_capacity;
}mc_msg_cfg;


#define mc_msg_cfg(IO, POOL_SIZE, WND_SIZE)\
	(mc_msg_cfg){.io = (IO), .recv_pool_size = (POOL_SIZE), .window_size = (WND_SIZE)}

mc_result_u32  mc_msg_get_alloc_size(mc_msg_cfg config);
mc_result_ptr  mc_msg_init(mc_buffer alloc_buffer, mc_msg_cfg config);
mc_error       mc_msg_update(mc_msg* this);// TODO(MN): receive
mc_error       mc_msg_subscribe(mc_msg* this, mc_msg_id id, mc_msg_receive_cb on_receive);
mc_error       mc_msg_unsubscribe(mc_msg* this, mc_msg_id id);
mc_result_u32  mc_msg_send(mc_msg* this, mc_buffer buffer, mc_msg_id id, uint32_t timeout_us);
mc_result_u32  mc_msg_signal(mc_msg* this, mc_msg_id id, uint32_t timeout_us);
mc_result_bool mc_msg_flush(mc_msg* this, uint32_t timeout_us);


#endif /* MC_IO_MESSAGE_H_ */
