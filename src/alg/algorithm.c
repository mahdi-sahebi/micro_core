#include <stddef.h>
#include <stdbool.h>
#include <math.h>
#include "alg/algorithm.h"


mc_u32 mc_alg_lower_bound(mc_buffer buffer, cvoid* data, mc_fn_distance fn_distance)
{
  mc_u32 result;

  if ((NULL == data) || (NULL == fn_distance) ||
      mc_buffer_is_null(buffer) || (0U == buffer.data_size)) {
    result = mc_u32(buffer.capacity, MC_ERR_INVALID_ARGUMENT);
  } else if (mc_buffer_is_empty(buffer)) {
    result = mc_u32(buffer.capacity, MC_SUCCESS);
  } else {
    uint32_t bgn = 0U;
    uint32_t end = buffer.capacity - 1U;
    bool     done = false;

    while ((bgn <= end) && !done) {
      cuint32_t mid = (bgn + end) >> 1U;
      const float distance = fn_distance(data, mc_buffer_at(buffer, mid));

      if        (distance > 0.0F) {
        if (buffer.capacity == mid) {
          bgn = mid;
          done = true;
        } else {
          bgn = mid + 1U;
        }
      } else if (distance < 0.0F) {
        if (0U == mid) {
          done = true;
        } else {
          end = mid - 1U;
        }
      } else {
        bgn = mid;
        done = true;
      }
    }

    result = mc_u32(bgn, MC_SUCCESS);
  }

  return result;
}

/* cppcheck-suppress misra-c2012-8.7 -- public API declared in algorithm.h */
mc_u32 mc_alg_nearest(mc_buffer buffer, cvoid* data, mc_fn_distance fn_distance)
{
  mc_u32 result = mc_alg_lower_bound(buffer, data, fn_distance);

  if (mc_is_ok(result) && (result.value != 0U)) {
    if (result.value == buffer.capacity) {
      result = mc_u32(buffer.capacity - 1U, MC_SUCCESS);
    } else {
      const float distance_cur = fn_distance(data, mc_buffer_at(buffer, result.value));
      const float distance_prv = fn_distance(data, mc_buffer_at(buffer, result.value - 1U));

      if (fabsf(distance_prv) < fabsf(distance_cur)) {
        result = mc_u32(result.value - 1U, MC_SUCCESS);
      }
    }
  }

  return result;
}

mc_u32 mc_alg_crc16_ccitt(mc_buffer buffer)
{
  mc_u32 result;

  if (mc_buffer_is_null(buffer)) {
    result = mc_u32(0U, MC_ERR_INVALID_ARGUMENT);
  } else {
    cuint32_t size = buffer.capacity * buffer.data_size;
    uint16_t crc = 0xFFFFU;

    for (uint32_t byte_index = 0U; byte_index < size; byte_index++) {
      crc = (uint16_t)(crc ^ (uint16_t)((uint16_t)((uint8_t)buffer.data[byte_index]) << 8U));

      for (uint8_t bit_index = 0U; bit_index < 8U; bit_index++) {
        cuint16_t shifted = (uint16_t)(crc << 1U);
        if (0U != (crc & 0x8000U)) {
          crc = (uint16_t)(shifted ^ 0x1021U);
        } else {
          crc = shifted;
        }
      }
    }

    result = mc_u32(crc, MC_SUCCESS);
  }

  return result;
}