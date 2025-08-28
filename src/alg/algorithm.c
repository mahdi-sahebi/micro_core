#include <stdlib.h>
#include "alg/algorithm.h"


mc_result_u32 mc_alg_lower_bound(mc_buffer buffer, const void* data, mc_distance_fn distance_fn)
{
  if ((NULL == data) || (NULL == distance_fn) || 
      mc_buffer_is_null(buffer) || (0 == buffer.data_size)) {
    return mc_result_u32(buffer.capacity, MC_ERR_INVALID_ARGUMENT);
  }

  if (mc_buffer_is_empty(buffer)) {
    return mc_result_u32(buffer.capacity, MC_SUCCESS);
  }

  uint32_t bgn = 0;
  uint32_t end = buffer.capacity - 1;

  while (bgn <= end) {
    const uint32_t mid = (bgn + end) >> 1;
    const float distance = distance_fn(data, (char*)buffer.data + (mid * buffer.data_size));

    if        (distance > 0.0F) {
      if (buffer.capacity == mid) {
        bgn = mid;
        break;
      }
      
      bgn = mid + 1;
    } else if (distance < 0.0F) {
      if (0 == mid) {
        break;
      }

      end = mid - 1;
    } else {
      bgn = mid;
      break;
    }
  }

  return mc_result_u32(bgn, MC_SUCCESS);
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