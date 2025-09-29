#ifndef MC_PATTERN_CHAIN_H_
#define MC_PATTERN_CHAIN_H_

#include <core/error.h>
#include <alg/mc_buffer.h>


typedef struct
{
  mc_buffer  buffer;
  mc_error error;
}mc_chain_data;

typedef mc_chain_data (*mc_cb_chain)(mc_buffer buffer, void* arg);

typedef struct
{
  mc_cb_chain api;
  void*       arg;
}mc_chain_node;

typedef struct  // TODO(MN): Remove pads. Align array
{
  uint8_t       capacity;
  uint8_t       count;
  mc_chain_node nodes[0];
}mc_chain;


// TODO(MN): Don't get error
#define mc_chain_data(BUFFER, ERROR)\
  (mc_chain_data){.buffer = (BUFFER), .error = (ERROR)}

#define mc_chain_data_error(ERROR)\
  mc_chain_data(mc_buffer_null(), ERROR)

mc_u32 mc_chain_get_alloc_size(uint8_t capacity);
mc_ptr mc_chain_init(mc_buffer alloc_buffer, uint8_t capacity);
mc_error      mc_chain_clear(mc_chain* this);
mc_error      mc_chain_push(mc_chain* this, mc_cb_chain api, void* arg);
mc_chain_data mc_chain_run(mc_chain* this, mc_buffer buffer);


#endif /* MC_PATTERN_CHAIN_H_ */
