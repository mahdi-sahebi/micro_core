#ifndef MC_ERROR_H_
#define MC_ERROR_H_

#include <stdbool.h>
#include <stdint.h>


typedef enum __attribute__((packed))
{
  MC_SUCCESS = 0,
  MC_ERR_BAD_ALLOC,
  MC_ERR_INVALID_ARGUMENT,
  MC_ERR_FULL,
  MC_ERR_OUT_OF_RANGE,
  MC_ERR_TIMEOUT,
  MC_ERR_INCOMPLETE,// TODO(MN): Remove
  MC_ERR_NO_SPACE,
  MC_ERR_RUNTIME
}mc_error;


typedef struct
{
  uint32_t value;
  mc_error error;
}mc_result_u32;

#define mc_result_u32(VALUE, ERROR)     (mc_result_u32){.value = (VALUE), .error = (ERROR)}


typedef struct
{
  bool     value;
  mc_error error;
}mc_result_bool;

#define mc_result_bool(BOOL, ERROR)     (mc_result_bool){.value = (BOOL), .error = (ERROR)}


typedef struct
{
  void*    data;// TODO(MN): Use uintptr_t and have only one mc_result struct?
  mc_error error;
}mc_result_ptr;

#define mc_result_ptr(DATA, ERROR)      (mc_result_ptr){.data = (DATA), .error = (ERROR)}

#define mc_result_is_ok(RESULT)         (MC_SUCCESS == (RESULT).error)

#endif /* MC_ERROR_H_ */

