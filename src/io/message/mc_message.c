/* TODO(MN): Authentication: Pre-shared keys - Dynamic keys
 * ID-base messages
 * Event-driven
 * Test: Allow to zero pool size for small messages without buffering
 * Don't set window_capacity, calculate according to the buffer size
 */

#include <stdlib.h>
#include "dsa/sarray.h"
#include "io/communication/communication.h"
#include "io/message/mc_message.h"


typedef struct __attribute__((packed))
{
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


#define MIN(X, Y)     (((X) < (Y)) ? (X) : (Y))

static float id_compare(const void* a, const void* b)
{
  const mc_msg_id id_1 = ((id_node*)a)->id;
  const mc_msg_id id_2 = ((id_node*)b)->id;
  return id_1 - id_2;
}

static bool is_header_received(const mc_msg* this)
{
  return (this->recv_pool_stored >= sizeof(pkt_hdr));
}

static uint32_t remaining_message_size(const mc_msg* const this, const pkt_hdr* const pkt)
{
  return (pkt->size - (this->recv_pool_stored - sizeof(pkt_hdr)));
}

static uint32_t remaining_header_size(const mc_msg* const this)
{
  return (sizeof(pkt_hdr) - this->recv_pool_stored);
}

static mc_error drop_message(mc_msg* const this)
{
  const pkt_hdr* const pkt = (pkt_hdr*)this->recv_pool.data;
  const uint32_t expected_size = pkt->size + sizeof(pkt_hdr);

  if (mc_buffer_get_size(this->recv_pool) < expected_size) {// Droping message until the end because receive pool is smaller than message size
    const uint32_t seg_size = MIN(mc_buffer_get_size(this->recv_pool) - sizeof(pkt_hdr), expected_size - this->recv_pool_stored);// TODO(MN): Check minimum of sizeof(pkt_hdr)
    mc_result_u32 res = mc_comm_recv(this->comm, this->recv_pool.data + sizeof(pkt_hdr), seg_size, 1000000);
    this->recv_pool_stored += res.value;
    if (this->recv_pool_stored == expected_size) {
      this->recv_pool_stored = 0;
    }

    return MC_ERR_NO_SPACE;
  }

  return MC_SUCCESS;
}

static bool is_message_stored(mc_msg* const this)
{
  const pkt_hdr* const pkt = (pkt_hdr*)this->recv_pool.data;
  const uint32_t expected_size = pkt->size + sizeof(pkt_hdr);

  if (this->recv_pool_stored != expected_size) {
    const uint32_t size = remaining_message_size(this, pkt);

    mc_result_u32 result = mc_comm_recv(this->comm, this->recv_pool.data + this->recv_pool_stored, size, 10000);
    if (!mc_result_is_ok(result) && (MC_ERR_TIMEOUT != result.error)) {
      return false;
    }
    this->recv_pool_stored += result.value; 
  }

  if (this->recv_pool_stored == expected_size) {
    this->recv_pool_stored = 0;// TODO(MN): Move before break
    return true;
  }

  return false;
}

static bool read_message_header(mc_msg* this)
{
  const uint32_t size = remaining_header_size(this);
  const mc_result_u32 result = mc_comm_recv(this->comm, this->recv_pool.data + this->recv_pool_stored, size, 10000);
  
  if (!mc_result_is_ok(result)) {
    return false;
  }

  this->recv_pool_stored += result.value;
  return true;
}

static void on_receive(const mc_msg* this, const pkt_hdr* const pkt)
{
  id_node temp_node = {.id = pkt->msg_id};
  const mc_result_ptr result = mc_sarray_find(this->ids, &temp_node);
  
  if (mc_result_is_ok(result) && (NULL != result.data)) {
    const id_node* const node = result.data;
    if (temp_node.id == node->id) {
      node->on_receive(node->id, mc_buffer(this->recv_pool.data + sizeof(pkt_hdr), pkt->size));
    }
  }
}

mc_result_u32 mc_msg_get_alloc_size(mc_msg_cfg config)
{ 
  if (0 == config.pool_size) {
    return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
  }

  mc_result_u32 result = mc_comm_get_alloc_size(*(mc_comm_cfg*)&config);
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

  const uint32_t size = sizeof(mc_msg) + comm_size + config.pool_size;
  return mc_result_u32(size, MC_SUCCESS);
}

mc_result_ptr mc_msg_init(mc_buffer alloc_buffer, mc_msg_cfg config)
{
  // handle if pkt->size or even pkt_hdr are larger than pool_size
  const mc_result_u32 result = mc_msg_get_alloc_size(config);
  if (MC_SUCCESS != result.error) {
    return mc_result_ptr(NULL, result.error);
  }
  if (mc_buffer_get_size(alloc_buffer) < result.value) {
    return mc_result_ptr(NULL, MC_ERR_BAD_ALLOC);
  }

  mc_msg* this = (mc_msg*)alloc_buffer.data;

  mc_comm_cfg* comm_config = (mc_comm_cfg*)&config;
  const mc_buffer comm_buffer = mc_buffer(
    alloc_buffer.data + sizeof(mc_msg), mc_comm_get_alloc_size(*comm_config).value);
  this->comm = mc_comm_init(comm_buffer, *comm_config).data;

  this->recv_pool = mc_buffer(mc_buffer_end(comm_buffer), config.pool_size);
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

  uint32_t size = 0;
  if (!is_header_received(this) && !read_message_header(this)) {
    return MC_SUCCESS;
  }

  if (!is_header_received(this)) {
    return MC_SUCCESS;
  }

  mc_error error = MC_SUCCESS;
  if (error = drop_message(this)) {
    return error;
  }

  if (is_message_stored(this)) {
    const pkt_hdr* const pkt = (pkt_hdr*)this->recv_pool.data;
    on_receive(this, pkt);
  }

  return MC_SUCCESS;
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

  return mc_result_u32(0, MC_SUCCESS);
}

mc_result_bool mc_msg_flush(mc_msg* this, uint32_t timeout_us)
{
  return mc_comm_flush(this->comm, timeout_us);
}


#undef MIN
