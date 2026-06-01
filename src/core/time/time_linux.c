#include <stddef.h>
#include <sys/time.h>
#include "core/time.h"

/* Explicit prototype for the POSIX call: under -std=c99 the declaration in
 * <sys/time.h> is gated behind a feature-test macro, and the MISRA checker
 * does not expand it. Declaring it here satisfies Rule 17.3 (no implicit
 * declaration) while remaining compatible with the system header. */
extern int gettimeofday(struct timeval* tv, void* tz);


mc_time_t mc_now(void)
{
  struct timeval now = {0};
  (void)gettimeofday(&now, NULL);

  return (((mc_time_t)now.tv_sec * 1000000U) + (mc_time_t)now.tv_usec) * 1000U;
}

mc_time_t mc_now_u(void)
{
  return mc_now() / 1000U;
}

mc_time_t mc_now_m(void)
{
  return mc_now() / 1000000U;
}

mc_time_t mc_now_s(void)
{
  return mc_now() / 1000000000U;
}
