#include <stddef.h>
#include <math.h>
#include "alg/algorithm.h"


mc_u32 mc_alg_lower_bound(mc_buffer buffer, cvoid* data, mc_fn_distance fn_distance)
{
  if ((NULL == data) || (NULL == fn_distance) ||
      mc_buffer_is_null(buffer) || (0U == buffer.data_size)) {
    return mc_u32(buffer.capacity, MC_ERR_INVALID_ARGUMENT);
  }

  if (mc_buffer_is_empty(buffer)) {
    return mc_u32(buffer.capacity, MC_SUCCESS);
  }

  uint32_t bgn = 0U;
  uint32_t end = buffer.capacity - 1U;

  while (bgn <= end) {
    cuint32_t mid = (bgn + end) >> 1U;
    const float distance = fn_distance(data, (char*)buffer.data + (mid * buffer.data_size));

    if        (distance > 0.0F) {
      if (buffer.capacity == mid) {
        bgn = mid;
        break;
      }
      
      bgn = mid + 1U;
    } else if (distance < 0.0F) {
      if (0U == mid) {
        break;
      }

      end = mid - 1U;
    } else {
      bgn = mid;
      break;
    }
  }

  return mc_u32(bgn, MC_SUCCESS);
}

mc_u32 mc_alg_nearest(mc_buffer buffer, cvoid* data, mc_fn_distance fn_distance)
{
  mc_u32 result = mc_alg_lower_bound(buffer, data, fn_distance);
  if (!mc_is_ok(result)) {
    return result;
  }

  if (result.value == 0U) {
    return result;
  }

  if (result.value == buffer.capacity) {
    return mc_u32(buffer.capacity - 1U, MC_SUCCESS);
  }

  const float distance_cur = fn_distance(data, (char*)buffer.data + (result.value * buffer.data_size));
  const float distance_prv = fn_distance(data, (char*)buffer.data + ((result.value - 1U) * buffer.data_size));
  if (fabsf(distance_prv) < fabsf(distance_cur)) {
    return mc_u32(result.value - 1U, MC_SUCCESS);
  }

  return mc_u32(result.value, MC_SUCCESS);
}

mc_u32 mc_alg_crc16_ccitt(mc_buffer buffer)
{
  if (mc_buffer_is_null(buffer)) {
    return mc_u32(0U, MC_ERR_INVALID_ARGUMENT);
  }

  uint32_t size = buffer.capacity * buffer.data_size;
  const uint8_t* itr = (const uint8_t*)buffer.data;
  uint16_t crc = 0xFFFFU;

  while (0U != size) {
    crc = (uint16_t)(crc ^ (uint16_t)((uint16_t)(*itr) << 8U));
    itr++;
    for (uint8_t index = 0U; index < 8U; index++) {
      cuint16_t shifted = (uint16_t)(crc << 1U);
      if (0U != (crc & 0x8000U)) {
        crc = (uint16_t)(shifted ^ 0x1021U);
      } else {
        crc = shifted;
      }
    }
    size--;
  }

  return mc_u32(crc, MC_SUCCESS);
}