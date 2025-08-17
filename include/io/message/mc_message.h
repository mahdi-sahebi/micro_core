#ifndef MC_IO_MESSAGE_H_
#define MC_IO_MESSAGE_H_

#include <stdint.h>
#include "core/error.h"

typedef struct _mc_msg mc_msg;


mc_result_u32 mc_msg_get_alloc_size();
mc_result_ptr mc_msg_init();
mc_error      mc_msg_update();
mc_error      mc_msg_subscribe();
mc_error      mc_msg_unsubscribe();
mc_result_u32 mc_msg_send();
mc_result_u32 mc_msg_recv();


#endif /* MC_IO_MESSAGE_H_ */
