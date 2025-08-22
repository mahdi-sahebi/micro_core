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
  // uint32_t  rest_size;
  uint16_t  size;
  mc_msg_id msg_id;
  char      data[0];
}pkt_hdr;

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
  const mc_msg_id id_1 = ((id_node*)a)->id;
  const mc_msg_id id_2 = ((id_node*)b)->id;
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
      mc_buffer_end(this->recv_pool), mc_sarray_required_size(sizeof(id_node), config.ids_capacity).value);
    this->ids = mc_sarray_init(ids_buffer, sizeof(id_node), config.ids_capacity, id_compare).data;
  }

  return mc_result_ptr(this, MC_SUCCESS);
}

mc_error mc_msg_update(mc_msg* this)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }

  mc_comm_update(this->comm);
// handle if pkt->size or even pkt_hdr are larger than recv_pool_size
  // Receive packet header.
  uint32_t size = 0;
  if (this->recv_pool_stored < sizeof(pkt_hdr)) {
    size = sizeof(pkt_hdr) - this->recv_pool_stored;

    mc_result_u32 result = mc_comm_recv(this->comm, this->recv_pool.data + this->recv_pool_stored, size, 10000);
    if (!mc_result_is_ok(result)) {
      return MC_SUCCESS;
    }
    this->recv_pool_stored += result.value; 
  }
  // TODO(MN): Stay here if at least on pkt_hdr is received or 1 byte. 
  
  mc_error error = MC_SUCCESS;

  if (this->recv_pool_stored >= sizeof(pkt_hdr)) {
    // TODO(MN): Stay here to receive all message
    const pkt_hdr* pkt = (pkt_hdr*)this->recv_pool.data;
    const uint32_t data_size = pkt->size;

    if (0 == pkt->size) {
      this->recv_pool_stored = 0;// TODO(MN): Move before break
      // Find the id and it's callback

      id_node temp_node = {.id = pkt->msg_id};
      const mc_result_ptr itr = mc_sarray_find(this->ids, &temp_node);
      if (mc_result_is_ok(itr) && (NULL != itr.data)) {
        const id_node* const node = itr.data;
        node->on_receive(node->id, mc_buffer(NULL, 0));
      }

      return MC_SUCCESS;
    }

    while (this->recv_pool_stored != (data_size + sizeof(pkt_hdr))) { 


      if (mc_buffer_get_size(this->recv_pool) < (data_size + sizeof(pkt_hdr))) {
        #define MIN(X, Y)     (((X) < (Y)) ? (X) : (Y))
        const uint32_t seg_size = MIN(mc_buffer_get_size(this->recv_pool), data_size + sizeof(pkt_hdr) - this->recv_pool_stored);
        mc_result_u32 res = mc_comm_recv(this->comm, this->recv_pool.data, seg_size, 1000000);
        this->recv_pool_stored += res.value;
        // error = MC_ERR_OUT_OF_RANGE; // TODO(MN): Not enough memory
        if (this->recv_pool_stored == (data_size + sizeof(pkt_hdr))) {
          this->recv_pool_stored = 0;
          break;
        }
        continue;
        #undef MIN
      }


      size = data_size - (this->recv_pool_stored - sizeof(pkt_hdr));
  // TODO(MN): store the pkt_hdr on a temp object and not on the recv_pool. retrn the not_enough_memory error
      mc_result_u32 result = mc_comm_recv(this->comm, this->recv_pool.data + this->recv_pool_stored, size, 10000);
      if (!mc_result_is_ok(result) && (MC_ERR_TIMEOUT != result.error)) {
        return MC_SUCCESS;
      }
      this->recv_pool_stored += result.value; 

      if (this->recv_pool_stored == (data_size + sizeof(pkt_hdr))) {
        this->recv_pool_stored = 0;// TODO(MN): Move before break
        // Find the id and it's callback

        id_node temp_node = {.id = pkt->msg_id};
        const mc_result_ptr itr = mc_sarray_find(this->ids, &temp_node);
        if (mc_result_is_ok(itr) && (NULL != itr.data)) {
          const id_node* const node = itr.data;
          node->on_receive(node->id, mc_buffer(this->recv_pool.data + sizeof(pkt_hdr), data_size));
        }
        
        break;
      }

    }

  }

  return error;
}

mc_error mc_msg_subscribe(mc_msg* this, mc_msg_id id, mc_msg_receive_cb on_receive)
{
  if ((NULL == this) || (NULL == on_receive)) {
    return MC_ERR_INVALID_ARGUMENT;
  }

  if (NULL == this->ids) {
    return MC_ERR_OUT_OF_RANGE;
  }

  id_node node = {.id = id, .on_receive = on_receive};
  return mc_sarray_insert(this->ids, &node);
}

mc_error mc_msg_unsubscribe(mc_msg* this, mc_msg_id id)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }

  if (NULL == this->ids) {
    return MC_ERR_OUT_OF_RANGE;
  }

  id_node node = {.id = id};
  return mc_sarray_remove(this->ids, &node);
}

mc_result_u32 mc_msg_send(mc_msg* this, mc_buffer buffer, mc_msg_id id, uint32_t timeout_us)
{// TODO(MN): Handle total timeout_us
  if ((NULL == this) || (mc_buffer_is_null(buffer))) {
    return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
  }

  const uint32_t size = mc_buffer_get_size(buffer);

  pkt_hdr pkt = {
    .size = size,
    .msg_id = id
  };
  
  mc_result_u32 result = mc_comm_send(this->comm, &pkt, sizeof(pkt), timeout_us);
  if (!mc_result_is_ok(result)) {
    return result;
  }

  result = mc_comm_send(this->comm, buffer.data, size, timeout_us);
  if (!mc_result_is_ok(result)) {
    return result;
  }

  const mc_result_bool result_bool = mc_comm_flush(this->comm, timeout_us);
  if (!mc_result_is_ok(result_bool)) {
    return mc_result_u32(0, result.error);
  }

  return mc_result_u32(size, MC_SUCCESS);
}

mc_result_u32 mc_msg_signal(mc_msg* this, mc_msg_id id, uint32_t timeout_us)
{// TODO(MN): Handle total timeout_us
  if (NULL == this) {
    return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
  }

  pkt_hdr pkt = {
    .size = 0,
    .msg_id = id
  };
  
  mc_result_u32 result = mc_comm_send(this->comm, &pkt, sizeof(pkt), timeout_us);
  if (!mc_result_is_ok(result)) {
    return result;
  }
// TODO(MN): Don't flush at the end of sends
  const mc_result_bool result_bool = mc_comm_flush(this->comm, timeout_us);
  if (!mc_result_is_ok(result_bool)) {
    return mc_result_u32(0, result.error);
  }

  return mc_result_u32(0, MC_SUCCESS);
}

mc_result_bool mc_msg_flush(mc_msg* this, uint32_t timeout_us)
{
  return mc_comm_flush(this->comm, timeout_us);
}
