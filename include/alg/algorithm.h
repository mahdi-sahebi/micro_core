#ifndef MC_ALG_ALGORITHM_H_
#define MC_ALG_ALGORITHM_H_

#include <stdint.h>
#include "alg/span.h"

typedef int8_t (*mc_alg_comparator)(const void* data_1, const void* data_2);


void* mc_alg_lower_bound(mc_span buffer, const void* data, uint16_t data_size, mc_alg_comparator cmp);


#endif /* MC_ALG_ALGORITHM_H_ */
