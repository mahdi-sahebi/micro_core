/* TODO(MN): typedef const uint32_t cuint32_t;
 * Reduce text size of this module.
 * TODO(MN): Rename 'value' to have unique name for all result types
 */
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
}mc_err;


typedef struct
{
  uint32_t value;
  mc_err error;
}mc_u32;

#define mc_u32(VALUE, ERROR)     ((mc_u32){.value = (VALUE), .error = (ERROR)})


typedef struct
{
  bool     value;
  mc_err error;
}mc_bool;

#define mc_bool(BOOL, ERROR)     ((mc_bool){.value = (BOOL), .error = (ERROR)})


typedef struct
{
  void*    data;
  mc_err error;
}mc_ptr;

#define mc_ptr(DATA, ERROR)      ((mc_ptr){.data = (DATA), .error = (ERROR)})

#define mc_is_ok(RESULT)         (MC_SUCCESS == (RESULT).error)


// TOOD(MN): Move to types.h
typedef const void cvoid;
typedef const char cchar;
typedef const uint8_t cuint8_t;
typedef const uint16_t cuint16_t;
typedef const uint32_t cuint32_t;
typedef const uint64_t cuint64_t;


#endif /* MC_ERROR_H_ */

