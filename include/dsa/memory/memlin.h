#ifndef MC_MEMORY_LINEAR_H_
#define MC_MEMORY_LINEAR_H_

// Memory Linear Allocation
/* can be checked, overflow detection, debug log, warning,... 
not dependent on malloc/free/heap
not fragmentable
*/

#include <core/error.h>
#include <alg/span.h>


typedef struct _mc_memlin mc_memlin;


// TODO(MN): warn_unused_result attribute for alloc memory APIs
mc_result_ptr  mc_memlin_create(const mc_span buffer);
mc_result      mc_memlin_destroy(mc_memlin** this);

mc_result_ptr  mc_memlin_alloc(mc_memlin* this, uint32_t size);
mc_result      mc_memlin_clear(mc_memlin* this);

mc_result_bool mc_memlin_is_empty(const mc_memlin* this);
mc_result_bool mc_memlin_is_full(const mc_memlin* this);

mc_result_u32  mc_memlin_get_capacity(const mc_memlin* this);
mc_result_u32  mc_memlin_get_size(const mc_memlin* this);
mc_result_u32  mc_memlin_get_meta_size(const mc_memlin* this);


#endif /* MC_MEMORY_LINEAR_H_ */

