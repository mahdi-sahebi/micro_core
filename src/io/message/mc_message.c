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
}msg_pkt;

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


static mc_cmp id_compare(const void* a, const void* b)
{
  const mc_msg_id id_1 = *(mc_msg_id*)a;
  const mc_msg_id id_2 = *(mc_msg_id*)b;
  return (id_1 < id_2) ? MC_ALG_LT : ((id_1 > id_2) ? MC_ALG_GT : MC_ALG_EQ);
}

mc_result_u32 mc_msg_get_alloc_size(mc_msg_cfg config)
{
  if ((NULL == config.io.recv) || (NULL == config.io.send) ||
    (0 == config.recv_pool_size) || (0 == config.window_size)) {
    return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
  }

  mc_result_u32 result = mc_comm_get_alloc_size(config.window_size, 3);
  if (MC_SUCCESS != result.error) {
    return result;
  }
  const uint32_t comm_size = result.value;

  uint32_t ids_size = 0;
  if (0 != config.ids_capacity) {
    result = mc_sarray_required_size(sizeof(id_node), config.ids_capacity);
    if (MC_SUCCESS != result.error) {
      return result;
    }
    ids_size = result.value;
  }

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

  const mc_buffer comm_buffer = mc_buffer(
    alloc_buffer.data + sizeof(mc_msg), mc_comm_get_alloc_size(config.window_size, 3).value);
  this->comm = mc_comm_init(comm_buffer, config.window_size, 3, config.io).data;

  this->recv_pool = mc_buffer(mc_buffer_end(comm_buffer), config.recv_pool_size);
  this->recv_pool_stored = 0;

  this->ids = NULL;
  if (0 != config.ids_capacity) {
    const mc_buffer ids_buffer = mc_buffer(
      mc_buffer_end(comm_buffer), mc_sarray_required_size(sizeof(id_node), config.ids_capacity).value);
    this->ids = mc_sarray_init(ids_buffer, sizeof(id_node), config.ids_capacity, id_compare).data;
  }

  return mc_result_ptr(this, MC_SUCCESS);
}

mc_error mc_msg_update(mc_msg* this)
{
  return mc_comm_update(this->comm);
}

mc_error mc_msg_subscribe(mc_msg* this, mc_msg_id id, mc_msg_receive_cb on_receive)
{
  return MC_SUCCESS;
}

mc_error mc_msg_unsubscribe(mc_msg* this, mc_msg_id id)
{
  return MC_SUCCESS;
}

mc_result_u32 mc_msg_send(mc_msg* this, mc_buffer buffer, mc_msg_id id, uint32_t timeout_us)
{
  return mc_result_u32(0, MC_SUCCESS);
}

mc_result_bool mc_msg_flush(mc_msg* this, uint32_t timeout_us)
{
  return mc_result_bool(false, MC_SUCCESS);
}
