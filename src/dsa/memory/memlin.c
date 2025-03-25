/*
typedef struct
{
    // bool    is_internal : 1;// Meaningful for memlin wich is the base of all containers
    uint8_t word_size   : 2;//0->u8,1->u16,2->u32,3->u64
    uint8_t __pad       : 6;// refrence count

    uint8_t capacity;
    uint8_t size;
    char    data[0];
}meta_08;

doc of meta data size for each capacity: meta_08/16/32.
*/

#include <stdlib.h>
#include "dsa/memory/memlin.h"


typedef void (*initiator)(void* const, uint64_t);

typedef enum __attribute__((packed))
{
    WORD_08_BITS = 0,
    WORD_16_BITS,
    WORD_32_BITS
}word_type;

struct _mc_memlin
{
    uint8_t word_size : 2;
    uint8_t __pad     : 6;
};

typedef struct
{
    uint8_t word_size : 2;
    uint8_t __pad     : 6;
    uint8_t capacity;
    uint8_t size;
    char    data[0];
}meta_08;

typedef struct __attribute__((packed))
{
    uint8_t  word_size : 2;
    uint8_t  __pad     : 6;
    uint16_t capacity;
    uint16_t size;
    char     data[0];
}meta_16;

typedef struct __attribute__((packed))
{
    uint8_t  word_size : 2;
    uint8_t  __pad     : 6;
    uint32_t capacity;
    uint32_t size;
    char     data[0];
}meta_32;



static inline word_type get_word_type(uint32_t num)
{
    word_type word = WORD_08_BITS;

    if (num & 0xffff0000) {
        word = WORD_32_BITS;
    } else if (num & 0xff00) {
        word = WORD_16_BITS;
    }

    return word;
}

static inline word_type get_word_size(const struct _mc_memlin* this)
{
    return this->word_size;
}

static uint8_t get_meta_size(uint32_t size)
{
    const uint8_t metasizes[] = {
        [WORD_08_BITS] = sizeof(meta_08),
        [WORD_16_BITS] = sizeof(meta_16),
        [WORD_32_BITS] = sizeof(meta_32),
    };

    word_type word = get_word_type(size);
    if ((WORD_08_BITS < word) && (get_word_type(size - metasizes[word]) < word)) {
        word--;
    }
    
    return metasizes[word];
}

static void init_08(void* const this, uint64_t size)
{
    ((meta_08*)this)->capacity = size - sizeof(meta_08);
    ((meta_08*)this)->size = 0;
}

static void init_16(void* const this, uint64_t size)
{
    ((meta_16*)this)->capacity = size - sizeof(meta_16);
    ((meta_16*)this)->size = 0;
}

static void init_32(void* const this, uint64_t size)
{
    ((meta_32*)this)->capacity = size - sizeof(meta_32);
    ((meta_32*)this)->size = 0;
}

static uint32_t get_capacity_08(const void* const this)
{
    return ((meta_08*)this)->capacity;
}

static uint32_t get_capacity_16(const void* const this)
{
    return ((meta_16*)this)->capacity;
}

static uint32_t get_capacity_32(const void* const this)
{
    return ((meta_32*)this)->capacity;
}

static uint32_t get_capacity(const void* this)
{
    typedef uint32_t (*getter_cb)(const void* const);
    const getter_cb getter[] = {
        [WORD_08_BITS] = get_capacity_08,
        [WORD_16_BITS] = get_capacity_16,
        [WORD_32_BITS] = get_capacity_32,
    };

    return getter[get_word_size(this)](this);
}

static uint32_t get_size_08(const void* const this)
{
    return ((meta_08*)this)->size;
}

static uint32_t get_size_16(const void* const this)
{
    return ((meta_16*)this)->size;
}

static uint32_t get_size_32(const void* const this)
{
    return ((meta_32*)this)->size;
}

static uint32_t get_size(const void* this)
{
    typedef uint32_t (*getter_cb)(const void* const);
    const getter_cb getter[] = {
        [WORD_08_BITS] = get_size_08,
        [WORD_16_BITS] = get_size_16,
        [WORD_32_BITS] = get_size_32,
    };

    return getter[get_word_size(this)](this);
}

static void set_size_08(void* const this, uint32_t size)
{
    ((meta_08*)this)->size = size;
}

static void set_size_16(void* const this, uint32_t size)
{
    ((meta_16*)this)->size = size;
}

static void set_size_32(void* const this, uint32_t size)
{
    ((meta_32*)this)->size = size;
}

static void set_size(void* this, uint32_t size)
{
    typedef void (*getter_cb)(void* const, uint32_t);
    const getter_cb getter[] = {
        [WORD_08_BITS] = set_size_08,
        [WORD_16_BITS] = set_size_16,
        [WORD_32_BITS] = set_size_32,
    };

    getter[get_word_size(this)](this, size);
}

static void* get_data_08(const void* const this, uint32_t index)
{
    return ((meta_08*)this)->data;
}

static void* get_data_16(const void* const this, uint32_t index)
{
    return ((meta_16*)this)->data;
}

static void* get_data_32(const void* const this, uint32_t index)
{
    return ((meta_32*)this)->data;
}

static void* get_data(const void* const this, uint32_t index)
{
    typedef void* (*getter_cb)(const void* const, uint32_t index);
    const getter_cb getter[] = {
        [WORD_08_BITS] = get_data_08,
        [WORD_16_BITS] = get_data_16,
        [WORD_32_BITS] = get_data_32,
    };

    return getter[get_word_size(this)](this, index);
}

mc_result_ptr mc_memlin_create(const mc_span buffer)
{
    if ((NULL == buffer.data) || (0 == buffer.size)) {
        return mc_result_ptr(NULL, MC_ERR_BAD_ALLOC);
    }
    
    const initiator inits[] = {
        [WORD_08_BITS] = init_08,
        [WORD_16_BITS] = init_16,
        [WORD_32_BITS] = init_32,
    };

    mc_memlin* const memory = (mc_memlin*)buffer.data;// remove
    const word_type word = get_word_type(buffer.size);
    memory->word_size = word;
    inits[word](memory, buffer.size);

    return mc_result_ptr(memory, MC_SUCCESS);
}

mc_result mc_memlin_destroy(mc_memlin** this)
{
    if ((NULL == this) || (NULL == *this)) {
        return MC_ERR_INVALID_ARGUMENT;
    }

    this = NULL;
    return MC_SUCCESS;
}

mc_result_ptr mc_memlin_alloc(mc_memlin* this, uint32_t size)
{
    if (NULL == this) {
        return mc_result_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
    }

    const uint32_t cur_size = get_size(this);
    if ((cur_size + size) > get_capacity(this)) {
        return mc_result_ptr(NULL, MC_ERR_BAD_ALLOC);
    }

    void* const data = get_data(this, cur_size);
    set_size(this, cur_size + size);
    return mc_result_ptr(data, MC_SUCCESS);
}

mc_result mc_memlin_clear(mc_memlin* this)
{
    if (NULL == this) {
        return MC_ERR_INVALID_ARGUMENT;
    }

    set_size(this, 0);
    return MC_SUCCESS;
}

mc_result_bool mc_memlin_is_empty(const mc_memlin* this)
{
    if (NULL == this) {
        return mc_result_bool(false, MC_ERR_INVALID_ARGUMENT);
    }

    return mc_result_bool(0 == get_size(this), MC_SUCCESS);
}

mc_result_bool mc_memlin_is_full(const mc_memlin* this)
{
    if (NULL == this) {
        return mc_result_bool(false, MC_ERR_INVALID_ARGUMENT);
    }

    return mc_result_bool(get_capacity(this) == get_size(this), MC_SUCCESS);
}

mc_result_u32 mc_memlin_get_capacity(const mc_memlin* this)
{
    if (NULL == this) {
        return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
    }

    return mc_result_u32(get_capacity(this), MC_SUCCESS);
}

mc_result_u32 mc_memlin_get_size(const mc_memlin* this)
{
    if (NULL == this) {
        return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
    }

    return mc_result_u32(get_size(this), MC_SUCCESS);
}

mc_result_u32 mc_memlin_get_meta_size(const mc_memlin* this)
{
    if (NULL == this) {
        return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
    }

    const struct _mc_memlin* const obj = (struct _mc_memlin*)this;

    const uint8_t META_SIZES[] = {
        [WORD_08_BITS] = sizeof(meta_08),
        [WORD_16_BITS] = sizeof(meta_16),
        [WORD_32_BITS] = sizeof(meta_32),
    };

    return mc_result_u32(META_SIZES[obj->word_size], MC_SUCCESS);
}