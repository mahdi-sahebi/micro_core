/* TODO(MN): Reduce meta data size.
 * Not tested. Implement test cases. How to find out the meta data size?
 * Remove capacity from mc_sarray_init. calculate according to the span?
 * Doc: memory safe for detaching pointer, destructor, free
 * Optimize mc_fn_distance
 */

#include <stddef.h>
#include <string.h>
#include "alg/mc_buffer.h"
#include "dsa/sarray.h"


struct mc_sarray_impl
{
  mc_fn_distance distance;
  uint32_t       capacity;
  uint32_t       count;
  uint16_t       data_size;
  char           data[];
};

static inline char* get_data(const mc_sarray this, uint32_t index)// TODO(MN): Opt
{
  return this->data + ((uint32_t)this->data_size * index);
}


// TODO(MN): Should it be meta_data_size/minimum_required_size
mc_u32 mc_sarray_required_size(uint32_t data_size, uint32_t capacity)// TODO(MN): u16,
{
  if ((0U == capacity) || (0U == data_size)) {
    return mc_u32(0U, MC_ERR_INVALID_ARGUMENT);
  }

  return mc_u32((uint32_t)(sizeof(struct mc_sarray_impl) + ((size_t)capacity * data_size)), MC_SUCCESS);
}

mc_ptr mc_sarray_init(mc_buffer buffer, uint32_t data_size, uint32_t capacity, mc_fn_distance distance)
{
  if (mc_buffer_is_null(buffer) || (0U == capacity) || (0U == data_size) || (NULL == distance)) {
    return mc_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
  }

  cuint32_t required_size = (uint32_t)(sizeof(struct mc_sarray_impl) + ((size_t)capacity * data_size));
  if (mc_buffer_get_size(buffer) < required_size) {
    return mc_ptr(NULL, MC_ERR_BAD_ALLOC);
  }

  mc_sarray this  = (mc_sarray)buffer.data;
  this->distance  = distance;
  this->capacity  = capacity;
  this->count     = 0U;
  this->data_size = (uint16_t)data_size;

  return mc_ptr(this, MC_SUCCESS);
}

mc_err mc_sarray_clear(mc_sarray this)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }

  this->count = 0U;

  return MC_SUCCESS;
}

mc_u32 mc_sarray_get_count(const mc_sarray this)
{
  if (NULL == this) {
    return mc_u32(0U, MC_ERR_INVALID_ARGUMENT);
  }

  return mc_u32(this->count, MC_SUCCESS);
}

mc_u32 mc_sarray_get_capacity(const mc_sarray this)
{
  if (NULL == this) {
    return mc_u32(0U, MC_ERR_INVALID_ARGUMENT);
  }

  return mc_u32(this->capacity, MC_SUCCESS);
}

mc_u32 mc_sarray_get_data_size(const mc_sarray this)
{
  if (NULL == this) {
    return mc_u32(0U, MC_ERR_INVALID_ARGUMENT);
  }

  return mc_u32(this->data_size, MC_SUCCESS);
}

mc_ptr mc_sarray_get(const mc_sarray this, uint32_t index)
{
  if (NULL == this) {
    return mc_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
  }
  if (index >= this->capacity) {
    return mc_ptr(NULL, MC_ERR_OUT_OF_RANGE);
  }

  return mc_ptr(get_data(this, index), MC_SUCCESS);
}

mc_ptr mc_sarray_find(const mc_sarray this, cvoid* const data)
{
  if ((NULL == this) || (NULL == data)) {
    return mc_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
  }

  if (0U == this->count) {
    return mc_ptr(NULL, MC_SUCCESS);
  }

  const mc_u32 result = mc_alg_lower_bound(
    mc_buffer_make(this->data, (uint32_t)this->data_size * this->count, this->data_size),
    data,
    this->distance);

  void* itr = (result.value == this->count) ? NULL : get_data(this, result.value);
  return mc_ptr(itr, MC_SUCCESS);
}

mc_err mc_sarray_insert(mc_sarray this, cvoid* data)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }
  if (this->count >= this->capacity) {
    return MC_ERR_OUT_OF_RANGE;
  }

  cuint32_t index = mc_alg_lower_bound(mc_buffer_make(this->data, (uint32_t)this->data_size * this->count, this->data_size), data, this->distance).value;
  if (index < this->count) {
    (void)memmove(get_data(this, index + 1U), get_data(this, index), (size_t)this->data_size * (this->count - index));
  }

  (void)memcpy(get_data(this, index), data, this->data_size);

  this->count++;
  return MC_SUCCESS;
}

mc_err mc_sarray_remove_at(mc_sarray this, uint32_t index)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }
  if (0U == this->count) {
    return MC_ERR_OUT_OF_RANGE;
  }

  (void)memmove(get_data(this, index), get_data(this, index + 1U), (size_t)this->data_size * (this->count - index));

  this->count--;
  return MC_SUCCESS;
}

mc_err mc_sarray_remove(mc_sarray this, cvoid* data)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }
  if (0U == this->count) {
    return MC_ERR_OUT_OF_RANGE;
  }

  const mc_ptr result = mc_sarray_find(this, data);
  if ((MC_SUCCESS != result.error) || (NULL == result.data)) {
    return result.error;
  }

  const ptrdiff_t byte_diff = (char*)result.data - this->data;
  const size_t byte_offset = (size_t)byte_diff;
  cuint32_t data_index = (uint32_t)(byte_offset / (size_t)this->data_size);
  (void)memmove(get_data(this, data_index),
          get_data(this, data_index + 1U),
          (size_t)this->data_size * (this->count - (data_index + 1U)));

  this->count--;
  return MC_SUCCESS;
}

mc_bool mc_sarray_is_empty(const mc_sarray this)
{
  if (NULL == this) {
    return mc_bool(false, MC_ERR_INVALID_ARGUMENT);
  }

  return mc_bool(0U == this->count, MC_SUCCESS);
}

mc_bool mc_sarray_is_full(const mc_sarray this)
{
  if (NULL == this) {
    return mc_bool(false, MC_ERR_INVALID_ARGUMENT);
  }

  return mc_bool(this->capacity == this->count, MC_SUCCESS);
}
