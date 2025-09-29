#ifndef MC_IO_MESSAGE_H_
#define MC_IO_MESSAGE_H_

#include <stdint.h>
#include "core/error.h"
#include "alg/mc_buffer.h"
#include "io/io.h"
#include "io/communication/communication.h"

typedef struct _mc_msg mc_msg;
typedef uint16_t mc_msg_id;
typedef void (*mc_msg_receive_cb)(mc_msg_id, mc_buffer);

typedef struct
{
  mc_io       io;
  mc_comm_wnd recv;
  mc_comm_wnd send;
  uint32_t    pool_size;
  uint16_t    ids_capacity;
}mc_msg_cfg;

#define mc_msg_cfg(IO, RECV_WND, SEND_WND, POOL_SIZE, IDS_CAPACITY)\
  (mc_msg_cfg){.io = (IO), .recv = (RECV_WND), .send = (SEND_WND), .pool_size = (POOL_SIZE), . ids_capacity = (IDS_CAPACITY)}


mc_u32  mc_msg_get_alloc_size(mc_msg_cfg config);
mc_ptr  mc_msg_init(mc_buffer alloc_buffer, mc_msg_cfg config);
mc_error       mc_msg_update(mc_msg* this);// TODO(MN): receive
mc_error       mc_msg_subscribe(mc_msg* this, mc_msg_id id, mc_msg_receive_cb on_receive);
mc_error       mc_msg_unsubscribe(mc_msg* this, mc_msg_id id);
mc_u32  mc_msg_send(mc_msg* this, mc_buffer buffer, mc_msg_id id, uint32_t timeout_us);
mc_u32  mc_msg_signal(mc_msg* this, mc_msg_id id, uint32_t timeout_us);
mc_bool mc_msg_flush(mc_msg* this, uint32_t timeout_us);


#endif /* MC_IO_MESSAGE_H_ */
