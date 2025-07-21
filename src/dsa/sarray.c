/* TODO(MN): Reduce meta data size.
 * Not tested. Implement test cases. How to find out the meta data size?
 */

#include <stdlib.h>
#include <string.h>
#include "dsa/span.h"
#include "dsa/sarray.h"


struct _mc_sarray
{
  mc_comparator comparator;
  uint32_t capacity;
  uint32_t count;
  uint16_t data_size;
  char     data[0];
};

#define GET_DATA(ARRAY, INDEX)   ((ARRAY)->data + ((ARRAY)->data_size * (INDEX)))


mc_result_ptr mc_sarray_init(mc_span buffer, uint32_t data_size, uint32_t capacity, mc_comparator comparator)
{
  if (mc_span_is_null(buffer) || mc_span_is_empty(buffer) ||
      (0 == capacity) || (0 == data_size) || (NULL == comparator)) {
    return mc_result_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
  }

  const uint32_t size = sizeof(struct _mc_sarray) + (capacity * data_size);
  if (buffer.size < size) {
    return mc_result_ptr(NULL, MC_ERR_OUT_OF_RANGE);
  }

  mc_sarray this   = (mc_sarray)buffer.data;
  this->comparator = comparator;
  this->capacity   = size;
  this->count      = 0;
  this->data_size  = data_size;

  return mc_result_ptr(this, MC_SUCCESS);
}

mc_result mc_sarray_clear(mc_sarray this)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }

  this->count = 0;

  return MC_SUCCESS;
}

mc_result_u32  mc_sarray_get_count(const mc_sarray this)
{
  if (NULL == this) {
    return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
  }

  return mc_result_u32(this->count, MC_SUCCESS);
}

mc_result_ptr mc_sarray_get(const mc_sarray this, uint32_t index)
{
  if (NULL == this) {
    return mc_result_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
  }
  if (index >= this->capacity) {
    return mc_result_ptr(NULL, MC_ERR_OUT_OF_RANGE);
  }

  return mc_result_ptr(GET_DATA(this, index), MC_ERR_INVALID_ARGUMENT);
}

mc_result mc_sarray_insert(mc_sarray this, const void* data)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }
  if (this->count >= this->capacity) {
    return MC_ERR_OUT_OF_RANGE;
  }

  if (0 == this->count) {
    memcpy(GET_DATA(this, 0), data, this->data_size);
  } else {
    uint32_t bgn = 0;
    uint32_t end = this->count;

    while (bgn < end) {
      const uint32_t mid = (bgn + end) >> 1;

      if (this->comparator(data, GET_DATA(this, mid))) {
        end = mid;
      } else {
        bgn = mid + 1;
      }
    }

    /* Shift one element to right */
    for (uint32_t index = this->count; index > bgn; index--) {
      memcpy(GET_DATA(this, index), GET_DATA(this, index - 1), this->data_size);
    }

    memcpy(GET_DATA(this, bgn), data, this->data_size);
  }

  this->count++;
  return MC_SUCCESS;
}

mc_result mc_sarray_remove(mc_sarray this, uint32_t index)
{
  if (NULL == this) {
    return MC_ERR_INVALID_ARGUMENT;
  }
  if (0 == this->count) {
    return MC_ERR_OUT_OF_RANGE;
  }

  /* Shift one element to left */
  for (uint32_t itr = index; itr < this->count - 1; itr++) {
    memcpy(GET_DATA(this, itr), GET_DATA(this, itr + 1), this->data_size);
  }

  this->count--;
  return MC_SUCCESS;
}

mc_result_bool mc_sarray_is_emtpy(const mc_sarray this)
{
  if (NULL == this) {
    return mc_result_bool(false, MC_ERR_INVALID_ARGUMENT);
  }

  return mc_result_bool(0 == this->count, MC_SUCCESS);
}

mc_result_bool mc_sarray_is_full(const mc_sarray this)
{
  if (NULL == this) {
    return mc_result_bool(false, MC_ERR_INVALID_ARGUMENT);
  }

  return mc_result_bool(this->capacity == this->count, MC_SUCCESS);
}


#undef GET_DATA
