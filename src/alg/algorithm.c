#include <stdlib.h>
#include "alg/algorithm.h"


mc_result_ptr mc_alg_lower_bound(mc_buffer buffer, const void* data, mc_cmp_fn comparator)
{
  if ((NULL == data) || (NULL == comparator) || 
      mc_buffer_is_null(buffer) || (0 == buffer.data_size)) {
    return mc_result_ptr(NULL, MC_ERR_INVALID_ARGUMENT);
  }

  if (mc_buffer_is_empty(buffer)) {
    return mc_result_ptr(NULL, MC_SUCCESS);
  }

  uint32_t bgn = 0;
  uint32_t end = buffer.capacity - 1;

  while (bgn <= end) {
    const uint32_t mid = (bgn + end) >> 1;
    const void* mid_element = (const char*)buffer.data + (mid * buffer.data_size);
    const mc_cmp cmp = comparator(data, mid_element);

    if (MC_ALG_GT == cmp) {
      bgn = mid + 1;
    } else if (MC_ALG_LT == cmp) {
      end = mid - 1;
    } else {
      return mc_result_ptr(mid_element, MC_SUCCESS);
    }
  }

  return mc_result_ptr(NULL, MC_SUCCESS);
}

mc_result_u32 mc_alg_crc16_ccitt(mc_buffer buffer)
{
  if (mc_buffer_is_null(buffer)) {
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