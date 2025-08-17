#ifndef MC_IO_MESSAGE_H_
#define MC_IO_MESSAGE_H_

#include <stdint.h>
#include "core/error.h"
<<<<<<< HEAD

typedef struct _mc_msg mc_msg;


mc_result_u32 mc_msg_get_alloc_size();
mc_result_ptr mc_msg_init();
mc_error      mc_msg_update();
mc_error      mc_msg_subscribe();
mc_error      mc_msg_unsubscribe();
mc_result_u32 mc_msg_send();
mc_result_u32 mc_msg_recv();
=======
#include "alg/span.h"

typedef struct _mc_msg mc_msg;
typedef uint16_t mc_msg_id;
typedef void (*mc_msg_receive_cb)(mc_msg_id, mc_span);


mc_result_u32  mc_msg_get_alloc_size(uint32_t recv_pool_size);
mc_result_ptr  mc_msg_init(uint32_t recv_pool_size);
mc_error       mc_msg_update(mc_msg* this);
mc_error       mc_msg_subscribe(mc_msg* this, mc_msg_id id, mc_msg_receive_cb on_receive);
mc_error       mc_msg_unsubscribe(mc_msg* this);
mc_result_u32  mc_msg_send(mc_msg* this);
mc_result_u32  mc_msg_recv(mc_msg* this);
mc_result_bool mc_msg_flush(mc_msg* this, uint32_t timeout_us);
>>>>>>> 9e322cb ([UPDATE]: APIs are declared)


#endif /* MC_IO_MESSAGE_H_ */
