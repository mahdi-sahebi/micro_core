/* TODO(MN): Reduce meta data size.
 * Not tested. Implement test cases.
 */

 #include <stdlib.h>
#include "dsa/span.h"
#include "dsa/sarray.h"


struct _mc_sarray
{
  uint32_t capacity;
  uint32_t count;
  uint16_t element_size;
  char     data[0];
};


mc_result mc_sarray_init(mc_sarray* const this, mc_span buffer, uint32_t element_size, uint32_t capacity, mc_comparator comparator)
{
  *this = NULL;
  if (mc_span_is_null(buffer) || mc_span_is_empty(buffer) ||
      (0 == capacity) || (0 == element_size) || (NULL == comparator)) {
    return MC_ERR_INVALID_ARGUMENT;
  }

  const uint32_t size = sizeof(struct _mc_sarray) + (capacity * element_size);

  if (buffer.size < size) {
    return MC_ERR_OUT_OF_RANGE;
  }

  *this                 = (mc_sarray)buffer.data;
  (*this)->capacity     = size;
  (*this)->count        = 0;
  (*this)->element_size = element_size;

  return MC_SUCCESS;
}

mc_result mc_sarray_clear(mc_sarray const this)
{
  return MC_ERR_INVALID_ARGUMENT;
}

mc_result_ptr mc_sarray_get(const mc_sarray const this, uint32_t index)
{
  return mc_result_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
}

mc_result mc_sarray_insert(mc_sarray const this, const void* const data, uint32_t index)
{
  return MC_ERR_INVALID_ARGUMENT;
}

mc_result mc_sarray_append(mc_sarray const this, const void* const data)
{
  return MC_ERR_INVALID_ARGUMENT;
}

mc_result mc_sarray_remove(mc_sarray const this, uint32_t index)
{
  return MC_ERR_INVALID_ARGUMENT;
}

