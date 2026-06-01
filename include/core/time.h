#ifndef MC_TIME_H_
#define MC_TIME_H_

#include <stdint.h>

typedef uint64_t mc_time_t;
#define MC_TIMEOUT_MAX      ((uint32_t)0xFFFFFFFFU)


mc_time_t mc_now(void);
mc_time_t mc_now_u(void);
mc_time_t mc_now_m(void);
mc_time_t mc_now_s(void);


#endif /* MC_TIME_H_ */
