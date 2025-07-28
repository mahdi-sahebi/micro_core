#ifndef MC_ALG_ALGORITHM_H_
#define MC_ALG_ALGORITHM_H_

#include <stdint.h>
#include "core/error.h"
#include "alg/span.h"


typedef enum __attribute__((packed))
{
  MC_ALG_EQ = 0,
  MC_ALG_GT = 1,
  MC_ALG_LT = 2
}mc_cmp;

typedef mc_cmp (*mc_cmp_fn)(const void* a, const void* b);


mc_result_ptr mc_alg_lower_bound(mc_span buffer, const void* data, mc_cmp_fn comparator);


#endif /* MC_ALG_ALGORITHM_H_ */
