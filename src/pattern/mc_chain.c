#include <pattern/mc_chain.h>



mc_result_u32 mc_chain_get_alloc_size(uint8_t capacity)
{
  if (0 == capacity) {
    return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
  }

  const uint32_t size = sizeof(mc_chain) + (sizeof(mc_chain_cb) * capacity);
  return mc_result_u32(size, MC_SUCCESS);
}

mc_result_ptr mc_chain_init(mc_span alloc_buffer, uint8_t capacity)
{
  if (mc_span_is_null(alloc_buffer) || mc_span_is_empty(alloc_buffer) || (0 == capacity)) {
    return mc_result_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
  }

  mc_chain* this = (mc_chain*)alloc_buffer.data;
  this->capacity = capacity;
  this->count = 0;
  return mc_result_ptr(this, MC_SUCCESS);
}

mc_error mc_chain_clear(mc_chain* this)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }

  this->count = 0;
  return MC_SUCCESS;
}

mc_error mc_chain_push(mc_chain* this, mc_chain_cb node)
{
  if ((NULL == this) || (NULL == node)) {
    return MC_ERR_INVALID_ARGUMENT;
  }

  if (this->count>= this->capacity) {
    return MC_ERR_OUT_OF_RANGE;
  }

  this->nodes[this->count] = node;
  this->count++;
  return MC_SUCCESS;
}

mc_chain_data mc_chain_run(mc_chain* this, mc_chain_data data)
{
  if (NULL == this) {
    return mc_chain_data(NULL, mc_span(NULL, 0), MC_ERR_INVALID_ARGUMENT);
  }

  for (uint8_t index = 0; (index < this->count) && (MC_SUCCESS != data.error); index++) {
    data = this->nodes[index](data);
  }

  return data;
}
