/* TODO(MN): Authentication: Pre-shared keys - Dynamic keys
 * ID-base messages
 * Event-driven
 */

#include <stdlib.h>
#include "io/message/mc_message.h"

typedef struct __attribute__((packed))
{
  uint32_t rest_size;
  mc_msg_id msg_id;
  char data[0];
}mc_msg_pkt;



mc_result_u32 mc_msg_get_alloc_size(uint32_t recv_pool_size)
{
  return mc_result_u32(0, MC_SUCCESS);
}

mc_result_ptr mc_msg_init(uint32_t recv_pool_size)
{
  return mc_result_ptr(NULL, MC_SUCCESS);
}

mc_error mc_msg_update(mc_msg* this)
{
  return MC_SUCCESS;
}

mc_error mc_msg_subscribe(mc_msg* this, mc_msg_id id, mc_msg_receive_cb on_receive)
{
  return MC_SUCCESS;
}

mc_error mc_msg_unsubscribe(mc_msg* this)
{
  return MC_SUCCESS;
}

mc_result_u32 mc_msg_send(mc_msg* this)
{
  return mc_result_u32(0, MC_SUCCESS);
}

mc_result_u32 mc_msg_recv(mc_msg* this)
{
  return mc_result_u32(0, MC_SUCCESS);
}

mc_result_bool mc_msg_flush(mc_msg* this, uint32_t timeout_us)
{
  return mc_result_bool(false, MC_SUCCESS);
}
