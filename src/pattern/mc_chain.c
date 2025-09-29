// TODO(MN): Don't store the nodes. get array of APIs and args in run()
#include <pattern/mc_chain.h>


mc_u32 mc_chain_get_alloc_size(uint8_t capacity)
{
  if (0 == capacity) {
    return mc_u32(0, MC_ERR_INVALID_ARGUMENT);
  }

  const uint32_t size = sizeof(mc_chain) + (sizeof(mc_chain_node) * capacity);
  return mc_u32(size, MC_SUCCESS);
}

mc_result_ptr mc_chain_init(mc_buffer alloc_buffer, uint8_t capacity)
{
  if (mc_buffer_is_null(alloc_buffer) || mc_buffer_is_empty(alloc_buffer) || (0 == capacity)) {
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

mc_error mc_chain_push(mc_chain* this, mc_chain_cb api, void* arg)
{
  if ((NULL == this) || (NULL == api)) {
    return MC_ERR_INVALID_ARGUMENT;
  }

  if (this->count>= this->capacity) {
    return MC_ERR_OUT_OF_RANGE;
  }

  this->nodes[this->count] = 
  (mc_chain_node){
    .api = api,
    .arg = arg
  };
  this->count++;
  return MC_SUCCESS;
}

mc_chain_data mc_chain_run(mc_chain* this, mc_buffer buffer)
{
  if (NULL == this) {
    return mc_chain_data_error(MC_ERR_INVALID_ARGUMENT);
  }

  mc_chain_data data = mc_chain_data(buffer, MC_SUCCESS);

  for (uint8_t index = 0; (index < this->count) && (MC_SUCCESS == data.error); index++) {
    mc_chain_node* const node = &this->nodes[index];
    data = node->api(data.buffer, node->arg);
  }

  return data;
}
