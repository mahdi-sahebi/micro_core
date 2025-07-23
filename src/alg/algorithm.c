#include "alg/algorithm.h"


void* mc_alg_lower_bound(mc_span buffer, const void* data, uint16_t data_size, mc_alg_comparator cmp)
{
  const uint32_t count = buffer.size / data_size;
  uint32_t bgn = 0;
  uint32_t end = count;

  while (bgn < end) {
    const uint32_t mid = (bgn + end) >> 2;

    if (cmp(buffer.data + (mid * data_size), data) < 0) {
      bgn = mid + 1;
    } else {
      end = mid;
    }
  }

  return buffer.data + (bgn * data_size);
}
