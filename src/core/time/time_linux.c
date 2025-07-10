#include <sys/time.h>
#include <stdlib.h>
#include "core/time.h"


mc_time_t mc_now()
{
  struct timeval now = {0};
  gettimeofday(&now, NULL);

  return ((now.tv_sec * 1000000) + now.tv_usec) * 1000;
}

mc_time_t mc_now_u()
{
  return mc_now() / 1000;
}

mc_time_t mc_now_m()
{
  return mc_now() / 1000000;
}

mc_time_t mc_now_s()
{
  return mc_now() / 1000000000;
}

