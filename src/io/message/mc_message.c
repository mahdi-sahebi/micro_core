/* TODO(MN): Authentication: Pre-shared keys - Dynamic keys
 * ID-base messages
 * Event-driven
 */

#include <stdlib.h>
#include "dsa/sarray.h"
#include "io/communication/communication.h"
#include "io/message/mc_message.h"
 
typedef struct __attribute__((packed))
{
  uint32_t  rest_size;
  uint16_t  size;
  mc_msg_id msg_id;
  char      data[0];
}mc_msg_pkt;

typedef struct __attribute__((packed))
{
  mc_msg_receive_cb on_receive;
  mc_msg_id         id;
}id_node;

struct _mc_msg
{
  mc_comm*  comm;
  mc_sarray ids;
  mc_buffer recv_pool;// TODO(MN): Light buffer with call fn on specific size?
  uint32_t  recv_pool_stored;
};


mc_result_u32 mc_msg_get_alloc_size(mc_msg_cfg config)
{
  if ((NULL == config.io.recv) || (NULL == config.io.send) ||
    (0 == config.recv_pool_size) || (0 == config.window_size) ||
    (0 == config.ids_capacity)) {
    return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
  }

  mc_result_u32 result = mc_comm_get_alloc_size(config.window_size, 3);
  if (MC_SUCCESS != result.error) {
    return result;
  }
  const uint32_t comm_size = result.value;

  result = mc_sarray_required_size(sizeof(id_node), config.ids_capacity);
  if (MC_SUCCESS != result.error) {
    return result;
  }
  const uint32_t ids_size = result.value;

  const uint32_t size = sizeof(mc_msg) + comm_size + config.recv_pool_size;
  return mc_result_u32(size, MC_SUCCESS);
}

mc_result_ptr mc_msg_init(mc_buffer alloc_buffer, mc_msg_cfg config)
{
  const mc_result_u32 result = mc_msg_get_alloc_size(config);
  if (MC_SUCCESS != result.error) {
    return mc_result_ptr(NULL, result.error);
  }
  if (mc_buffer_get_size(alloc_buffer) < result.value) {
    return mc_result_ptr(NULL, MC_ERR_BAD_ALLOC);
  }

  mc_msg* this = (mc_msg*)alloc_buffer.data;
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
