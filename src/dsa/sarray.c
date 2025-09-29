/* TODO(MN): Reduce meta data size.
 * Not tested. Implement test cases. How to find out the meta data size?
 * Remove capacity from mc_sarray_init. calculate according to the span?
 * Doc: memory safe for detaching pointer, destructor, free
 */

#include <stdlib.h>
#include <string.h>
#include "alg/mc_buffer.h"
#include "dsa/sarray.h"


struct _mc_sarray
{
  mc_distance_fn distance;
  uint32_t       capacity;
  uint32_t       count;
  uint16_t       data_size;
  char           data[0];
};

#define GET_DATA(ARRAY, INDEX)   ((ARRAY)->data + ((ARRAY)->data_size * (INDEX)))// TODO(MN): Opt


// TODO(MN): Should it be meta_data_size/minimum_required_size
mc_u32 mc_sarray_required_size(uint32_t data_size, uint32_t capacity)// TODO(MN): u16, 
{
  if ((0 == capacity) || (0 == data_size)) {
    return mc_u32(0, MC_ERR_INVALID_ARGUMENT);
  }

  return mc_u32(sizeof(struct _mc_sarray) + (capacity * data_size), MC_SUCCESS);
}

mc_ptr mc_sarray_init(mc_buffer buffer, uint32_t data_size, uint32_t capacity, mc_distance_fn distance)
{
  if (mc_buffer_is_null(buffer) || (0 == capacity) || (0 == data_size) || (NULL == distance)) {
    return mc_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
  }

  const uint32_t required_size = sizeof(struct _mc_sarray) + (capacity * data_size);
  if (mc_buffer_get_size(buffer) < required_size) {
    return mc_ptr(NULL, MC_ERR_BAD_ALLOC);
  }

  mc_sarray this  = (mc_sarray)buffer.data;
  this->distance  = distance;
  this->capacity  = capacity;
  this->count     = 0;
  this->data_size = data_size;

  return mc_ptr(this, MC_SUCCESS);
}

mc_error mc_sarray_clear(mc_sarray this)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }

  this->count = 0;

  return MC_SUCCESS;
}

mc_u32 mc_sarray_get_count(const mc_sarray this)
{
  if (NULL == this) {
    return mc_u32(0, MC_ERR_INVALID_ARGUMENT);
  }

  return mc_u32(this->count, MC_SUCCESS);
}

mc_u32 mc_sarray_get_capacity(const mc_sarray this)
{
  if (NULL == this) {
    return mc_u32(0, MC_ERR_INVALID_ARGUMENT);
  }

  return mc_u32(this->capacity, MC_SUCCESS);
}

mc_u32 mc_sarray_get_data_size(const mc_sarray this)
{
  if (NULL == this) {
    return mc_u32(0, MC_ERR_INVALID_ARGUMENT);
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

  return mc_ptr(GET_DATA(this, index), MC_SUCCESS);
}

mc_ptr mc_sarray_find(const mc_sarray this, const void* const data)
{
  if ((NULL == this) || (NULL == data)) {
    return mc_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
  }

  if (0 == this->count) {
    return mc_ptr(NULL, MC_SUCCESS);
  }
  
  const mc_u32 result = mc_alg_lower_bound(
    mc_buffer_raw(this->data, this->data_size * this->count, this->data_size), 
    data, 
    this->distance);
  
  void* itr = (result.value == this->count) ? NULL :
    this->data + (result.value * this->data_size);
  return mc_ptr(itr, MC_SUCCESS);
}

mc_error mc_sarray_insert(mc_sarray this, const void* data)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }
  if (this->count >= this->capacity) {
    return MC_ERR_OUT_OF_RANGE;
  }

  const uint32_t index = mc_alg_lower_bound(mc_buffer_raw(this->data, this->data_size * this->count, this->data_size), data, this->distance).value;
  if (index < this->count) {
    memmove(GET_DATA(this, index + 1), GET_DATA(this, index), this->data_size * (this->count - index));
  }

  memcpy(GET_DATA(this, index), data, this->data_size);

  this->count++;
  return MC_SUCCESS;
}

mc_error mc_sarray_remove_at(mc_sarray this, uint32_t index)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }
  if (0 == this->count) {
    return MC_ERR_OUT_OF_RANGE;
  }

  memmove(GET_DATA(this, index), GET_DATA(this, index + 1), this->data_size * (this->count - index));

  this->count--;
  return MC_SUCCESS;
}

mc_error mc_sarray_remove(mc_sarray this, const void* data)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }
  if (0 == this->count) {
    return MC_ERR_OUT_OF_RANGE;
  }

  const mc_ptr result = mc_sarray_find(this, data);
  if ((MC_SUCCESS != result.error) || (NULL == result.data)) { 
    return result.error;
  }

  const uint32_t data_index = ((char*)result.data - this->data) / this->data_size;
  memmove(this->data + (data_index * this->data_size),
          this->data + (data_index + 1) * this->data_size,
          (this->count - (data_index + 1)) * this->data_size);

  this->count--;
  return MC_SUCCESS;
}

mc_bool mc_sarray_is_empty(const mc_sarray this)
{
  if (NULL == this) {
    return mc_bool(false, MC_ERR_INVALID_ARGUMENT);
  }

  return mc_bool(0 == this->count, MC_SUCCESS);
}

mc_bool mc_sarray_is_full(const mc_sarray this)
{
  if (NULL == this) {
    return mc_bool(false, MC_ERR_INVALID_ARGUMENT);
  }

  return mc_bool(this->capacity == this->count, MC_SUCCESS);
}


#undef GET_DATA
