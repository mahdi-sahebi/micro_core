// TODO(MN): Namespace mc_err
#ifndef MC_DSA_SORTED_ARRAY_H_
#define MC_DSA_SORTED_ARRAY_H_

#include <stdint.h>
#include <stdbool.h>
#include "core/error.h"
#include "alg/mc_buffer.h"
#include "alg/algorithm.h"


typedef struct _mc_sarray* mc_sarray;

// TODO(MN): Rename to get_alloc_size
mc_result_u32  mc_sarray_required_size(uint32_t data_size, uint32_t capacity);
mc_result_ptr  mc_sarray_init(mc_buffer buffer, uint32_t data_size, uint32_t capacity, mc_cmp_fn comparator);// TODO(MN): Swap cap and data size
mc_error       mc_sarray_clear(mc_sarray this);
mc_result_u32  mc_sarray_get_count(const mc_sarray this);
mc_result_u32  mc_sarray_get_capacity(const mc_sarray this);
mc_result_u32  mc_sarray_get_data_size(const mc_sarray this);
mc_result_ptr  mc_sarray_get(const mc_sarray this, uint32_t index);
mc_result_ptr  mc_sarray_find(const mc_sarray this, const void* const data);
mc_error       mc_sarray_insert(mc_sarray this, const void* data);
mc_error       mc_sarray_remove_at(mc_sarray this, uint32_t index);
mc_error       mc_sarray_remove(mc_sarray this, const void* data);
mc_result_bool mc_sarray_is_empty(const mc_sarray this);
mc_result_bool mc_sarray_is_full(const mc_sarray this);


#endif /* MC_DSA_SORTED_ARRAY_H_ */
