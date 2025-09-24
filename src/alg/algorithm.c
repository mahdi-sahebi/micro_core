#include <stdlib.h>
#include <math.h>
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

mc_result_u32 mc_alg_nearest(mc_buffer buffer, const void* data, mc_distance_fn distance_fn)
{
  mc_result_u32 result = mc_alg_lower_bound(buffer, data, distance_fn);
  if (!mc_result_is_ok(result)) {
    return result;
  }

  if (result.value == 0) {
    return result;
  }

  if (result.value == buffer.capacity) {
    return mc_result_u32(buffer.capacity - 1, MC_SUCCESS);
  }

  const float distance_cur = distance_fn(data, (char*)buffer.data + ((result.value - 0) * buffer.data_size));
  const float distance_prv = distance_fn(data, (char*)buffer.data + ((result.value - 1) * buffer.data_size));
  if (fabsf(distance_prv) < fabsf(distance_cur)) {
    return mc_result_u32(result.value - 1, MC_SUCCESS);
  }

  return mc_result_u32(result.value, MC_SUCCESS);
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

mc_result_u32 mc_alg_crc32(mc_buffer buffer)
{
  uint32_t crc = 0xffffffff;
  uint32_t size = buffer.capacity * buffer.data_size;
  const uint8_t* itr = buffer.data;

  for (size_t i = 0; i < size; i++) {
    crc ^= itr[i];
    for (int j = 8; j > 0; j--) {
      if (crc & 1)
          crc = (crc >> 1) ^ 0xEDB88320;
      else
          crc >>= 1;
    }
  }

  crc = ~crc;
  return mc_result_u32(crc, MC_SUCCESS);
}
