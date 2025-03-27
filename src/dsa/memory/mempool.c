#include <stdlib.h>
#include "dsa/memory/memlin.h"
#include "dsa/memory/mempool.h"


//////////////////////////////////////////////////////////////////////////////////
// TODO(MN): Do it for other containers
// typedef struct
// {
//     uint8_t  capacity;
//     uint8_t  count;
//     uint8_t  data_size;
//     uint8_t  free_index;
//     uint8_t* free_indices;
//     //   char*    data;
// }meta_data_8;

// typedef struct
// {
//     uint16_t  capacity;
//     uint16_t  count;
//     uint16_t  data_size;
//     uint16_t  free_index;
//     uint16_t* free_indices;
//     //   char*     data;
// }meta_data_16;

// typedef struct
// {
//     uint32_t  capacity;
//     uint32_t  count;
//     uint32_t  data_size;
//     uint32_t  free_index;
//     uint32_t* free_indices;
//     //   char*     data;
// }meta_data_32;

// typedef struct
// {
//     uint64_t  capacity;
//     uint64_t  count;
//     uint64_t  data_size;
//     uint64_t  free_index;
//     uint64_t* free_indices;
//     //   char*     data;
// }meta_data_64;

// struct _mc_mempool
// {
//     uint8_t      word_size : 2;
//     uint8_t      __pad     : 6;
//     meta_data_8  meta_8[0];
//     meta_data_16 meta_16[0];
//     meta_data_32 meta_32[0];
//     meta_data_64 meta_64[0];
//     // char*     data;
// };
struct _mc_mempool
{
    uint32_t  capacity;
    uint32_t  count;
    uint32_t  data_size;
    uint32_t  free_index;
    uint32_t* free_indices;
    char*     data;
};


static void init_indices(mc_mempool* this)
{
    this->free_index = 0;

    for (uint32_t index = 0; index < (this->count - 1); index++) {
        this->free_indices[index] = index + 1;
    }

    this->free_indices[this->count - 1] = (uint32_t)-1;
}

mc_result_ptr mc_mempool_create(uint32_t capacity, uint16_t data_size)
{
    if ((0 == capacity) || (0 == data_size)) {
        return mc_result_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
    }

    mc_mempool* mem_pool = NULL;
    char* const buffer = malloc(sizeof(struct _mc_mempool) + 
                            (capacity * sizeof(*(mem_pool->free_indices))) + 
                            (capacity * sizeof(data_size)));
    if (NULL == buffer) {
        return mc_result_ptr(NULL, MC_ERR_BAD_ALLOC);
    }

    mem_pool = (mc_mempool*)buffer;
    mem_pool->capacity = capacity;
    mem_pool->count = 0;
    mem_pool->free_indices = (void*)(buffer + sizeof(struct _mc_mempool));
    mem_pool->data = (char*)mem_pool->free_indices + (capacity * sizeof(*(mem_pool->free_indices)));

    init_indices(mem_pool);

    return mc_result_ptr(mem_pool, MC_SUCCESS);
}

mc_result mc_mempool_destroy(mc_mempool** this)
{
    if ((NULL == this) || (NULL == *this)) {
        return MC_ERR_INVALID_ARGUMENT;
    }

    free(*this);
    this = NULL;

    return MC_SUCCESS;
}

mc_result_ptr mc_mempool_alloc(mc_mempool* this)
{
    if (NULL == this) {
        return mc_result_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
    }

    mc_result_ptr result = mc_result_ptr(NULL, MC_SUCCESS);

    return result;
}

mc_result mc_mempool_free(mc_mempool* this, void* data)
{
    mc_result result = MC_SUCCESS;

    return result;
}

mc_result_bool mc_mempool_is_full(const mc_mempool* this)
{
    if (NULL == this) {
        return mc_result_bool(false, MC_ERR_INVALID_ARGUMENT);
    }

    return mc_result_bool(false, MC_SUCCESS);
}

mc_result_bool mc_mempool_is_empty(const mc_mempool* this)
{
    if (NULL == this) {
        return mc_result_bool(false, MC_ERR_INVALID_ARGUMENT);
    }

    return mc_result_bool(false, MC_SUCCESS);
}

mc_result_u32 mc_mempool_get_capacity(const mc_mempool* this)
{
    mc_result_u32 result = mc_result_u32(0, MC_SUCCESS);

    return result;
}

mc_result_u32 mc_mempool_get_count(const mc_mempool* this)
{
    mc_result_u32 result = mc_result_u32(0, MC_SUCCESS);

    return result;
}

mc_result_u32 mc_mempool_get_data_size(const mc_mempool* this)
{
    mc_result_u32 result = mc_result_u32(0, MC_SUCCESS);

    return result;
}