#ifndef MC_DSA_SORTED_ARRAY_H_
#define MC_DSA_SORTED_ARRAY_H_

#include <stdint.h>
#include <stdbool.h>
#include "core/error.h"


typedef struct _mc_sarray* mc_sarray;
typedef bool (*mc_comparator)(const void* const data_1, const void* const data_2);


mc_result_ptr  mc_sarray_init(mc_span buffer, uint32_t data_size, uint32_t capacity, mc_comparator comparator);
mc_result      mc_sarray_clear(mc_sarray this);
mc_result_u32  mc_sarray_get_count(const mc_sarray this);
mc_result_ptr  mc_sarray_get(const mc_sarray this, uint32_t index);
mc_result      mc_sarray_insert(mc_sarray this, const void* data);
mc_result      mc_sarray_remove(mc_sarray this, uint32_t index);
mc_result_bool mc_sarray_is_emtpy(const mc_sarray this);
mc_result_bool mc_sarray_is_full(const mc_sarray this);


#endif /* MC_DSA_SORTED_ARRAY_H_ */
