#include <stdlib.h>
#include "alg/algorithm.h"


mc_result_ptr mc_alg_lower_bound(mc_span buffer, const void* data, mc_cmp_fn comparator)
{
  if ((NULL == data) || (NULL == comparator) || 
      mc_span_is_null(buffer) || (0 == buffer.data_size)) {
    return mc_result_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
  }

  if (mc_span_is_empty(buffer)) {
    return mc_result_ptr(NULL, MC_SUCCESS);
  }

  const uint32_t count = buffer.capacity / buffer.data_size;
  uint32_t bgn = 0;
  uint32_t end = count;

  while (bgn < end) {
    const uint32_t mid = (bgn + end) >> 1;

    if (comparator(buffer.data + (mid * buffer.data_size), data) < 0) {
      bgn = mid + 1;
    } else {
      end = mid;
    }
  }

  void* element = (bgn != count) ? (buffer.data + (bgn * buffer.data_size)) : NULL;
  return mc_result_ptr(element, MC_SUCCESS);
}

mc_result_u32 mc_alg_crc16_ccitt(mc_span buffer)
{
  if (mc_span_is_null(buffer)) {
    return mc_result_u32(0, MC_ERR_INVALID_ARGUMENT);
  }

  uint32_t size = buffer.capacity * buffer.data_size;
  const uint8_t* itr = buffer.data;
  uint16_t crc = 0xffff;

  while (size--) {
      crc ^= *itr++ << 8;
      for (uint8_t i = 0; i < 8; i++) {
          crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
      }
  }
  
  return mc_result_u32(crc, MC_SUCCESS);
}