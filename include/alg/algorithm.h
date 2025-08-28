#ifndef MC_ALG_ALGORITHM_H_
#define MC_ALG_ALGORITHM_H_

#include <stdint.h>
#include "core/error.h"
#include "alg/mc_buffer.h"


typedef float (*mc_distance_fn)(const void* a, const void* b);


// TODO(MN): Separate and move to search module
mc_result_u32 mc_alg_lower_bound(mc_buffer buffer, const void* data, mc_distance_fn distance_fn);
mc_result_u32 mc_alg_nearest(mc_buffer buffer, const void* data, mc_distance_fn distance_fn);

// TODO(MN): Separate and move to error module
mc_result_u32 mc_alg_crc16_ccitt(mc_buffer buffer);


#endif /* MC_ALG_ALGORITHM_H_ */
