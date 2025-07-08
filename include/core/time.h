#ifndef MC_TIME_H_
#define MC_TIME_H_

#include <stdint.h>

typedef uint64_t mc_time_t;

mc_time_t mc_now();
mc_time_t mc_now_u();
mc_time_t mc_now_m();
mc_time_t mc_now_s();

#endif /* MC_TIME_H_ */
