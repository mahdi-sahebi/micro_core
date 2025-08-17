#include <stdio.h>
#include "core/error.h"
#include "core/version.h"
#include "core/time.h"
#include "io/message/mc_message.h"


static int invalid_creation()
{
  return MC_ERR_RUNTIME;
}

static int valid_creation()
{
  return MC_ERR_RUNTIME;
}

static int authentication()
{
  return MC_ERR_RUNTIME;
}

static int singly_direction()// TODO(MN): Signal, bool, string, variadic, larger than pool,
{
  return MC_ERR_RUNTIME;
}

static int singly_repetitive()
{
  return MC_ERR_RUNTIME;
}

static int singly_low_lossy()
{
  return MC_ERR_RUNTIME;
}

static int singly_high_lossy()
{
  return MC_ERR_RUNTIME;
}

int main()
{
  printf("[MICRO CORE %u.%u.%u - IO - MESSAGE]\n", MC_VERSION_MAJOR, MC_VERSION_MINOR, MC_VERSION_PATCH);
  mc_error result = MC_SUCCESS;

  printf("[invalid_creation]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    result = invalid_creation();
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[valid_creation]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    result = valid_creation();
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[invalid_creation]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    result = invalid_creation();
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  return MC_SUCCESS;
}
