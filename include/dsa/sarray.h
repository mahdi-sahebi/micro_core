#ifndef MC_DSA_SORTED_ARRAY_H_
#define MC_DSA_SORTED_ARRAY_H_

#include <stdint.h>
#include <stdbool.h>
#include "core/error.h"


typedef struct _mc_sarray* mc_sarray;
typedef bool (*mc_comparator)(const void* const data_1, const void* const data_2);


mc_result     mc_sarray_init(mc_sarray* const this, mc_span buffer, uint32_t element_size, uint32_t capacity, mc_comparator comparator);
mc_result     mc_sarray_clear(mc_sarray const this);
mc_result_ptr mc_sarray_get(const mc_sarray const this, uint32_t index);
mc_result     mc_sarray_insert(mc_sarray const this, const void* const data, uint32_t index);
mc_result     mc_sarray_append(mc_sarray const this, const void* const data);
mc_result     mc_sarray_remove(mc_sarray const this, uint32_t index);


#endif /* MC_DSA_SORTED_ARRAY_H_ */
