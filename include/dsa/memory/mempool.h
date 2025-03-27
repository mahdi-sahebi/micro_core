#ifndef MC_MEMORY_POOL_H_
#define MC_MEMORY_POOL_H_

/** Memory Pool
 * 
 */

#include <core/error.h>
#include <dsa/span.h>


typedef struct _mc_mempool mc_mempool;

// create on memlin
// TODO(MN): void* type of object pointer
mc_result_ptr  mc_mempool_create_on_buffer(uint32_t capacity, uint16_t data_size, const mc_span buffer);
mc_result_ptr  mc_mempool_create(uint32_t capacity, uint16_t data_size);
mc_result      mc_mempool_destroy(mc_mempool** this);

mc_result_ptr  mc_mempool_alloc(mc_mempool* this);
mc_result      mc_mempool_free(mc_mempool* this, void* data);

mc_result_bool mc_mempool_is_full(const mc_mempool* this);
mc_result_bool mc_mempool_is_empty(const mc_mempool* this);

mc_result_u32  mc_mempool_get_capacity(const mc_mempool* this);
mc_result_u32  mc_mempool_get_count(const mc_mempool* this);
mc_result_u32  mc_mempool_get_data_size(const mc_mempool* this);
// TODO(MN): Serialize, deserialize for all containers


#endif /* MC_MEMORY_POOL_H_ */

