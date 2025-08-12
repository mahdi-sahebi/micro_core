#ifndef MC_PATTERN_CHAIN_H_
#define MC_PATTERN_CHAIN_H_

#include <core/error.h>
#include <alg/span.h>


typedef struct
{
  void*    arg;
  mc_span  buffer;
  mc_error error;
}mc_chain_data;

typedef mc_chain_data (*mc_chain_cb)(mc_chain_data);

typedef struct __attribute__((packed))
{
  uint8_t     capacity;
  uint8_t     count;
  mc_chain_cb nodes[0];
}mc_chain;


mc_result_u32 mc_chain_get_alloc_size(uint8_t capacity);
mc_result_ptr mc_chain_init(mc_span alloc_buffer, uint8_t capacity);
mc_error      mc_chain_clear(mc_chain* this);
mc_error      mc_chain_push(mc_chain* this, mc_chain_cb node);
mc_chain_data mc_chain_run(mc_chain* this, mc_chain_data data);


#endif /* MC_PATTERN_CHAIN_H_ */
