#ifndef MC_ERROR_H_
#define MC_ERROR_H_


#include <stdbool.h>
#include <stdint.h>


typedef enum __attribute__((packed))
{
    MC_SUCCESS = 0,
    MC_ERR_BAD_ALLOC,
    MC_ERR_INVALID_ARGUMENT,
    MC_ERR_MEMORY_FULL,
    MC_ERR_MEMORY_OUT_OF_RANGE,
    MC_ERR_TIMEOUT,
    MC_ERR_RUNTIME
}mc_result;


typedef struct
{
    uint32_t  value;
    mc_result result;
}mc_result_u32;

#define mc_result_u32(VALUE, ERROR)         (mc_result_u32){.value = (VALUE), .result = (ERROR)}


typedef struct
{
    bool     value;
    mc_result result;
}mc_result_bool;

#define mc_result_bool(BOOL, ERROR)         (mc_result_bool){.value = (BOOL), .result = (ERROR)}


typedef struct
{
    void*    data;
    mc_result result;
}mc_result_ptr;

#define mc_result_ptr(DATA, ERROR)          (mc_result_ptr){.data = (DATA), .result = (ERROR)}


#endif /* MC_ERROR_H_ */

