#ifndef MC_IO_COMMUNICATION_BASE_H_
#define MC_IO_COMMUNICATION_BASE_H_

#include <stdint.h>
#include "io/communication/mc_communication.h"
// #include "mc_io.h"
#include "mc_frame.h"


struct mc_comm_impl
{
  mc_frame* rcv;// TODO(MN): Use array to reduce one pointer size
  mc_frame* snd;
  mc_io     io;
  uint32_t  send_delay_us;// TODO(MN): Use u16 with 100X us resolution
};

#define MAX_SEND_TIME_US    1000000U
#define MIN_SEND_TIME_US    100U

static inline uint32_t comm_min_u32(uint32_t lhs, uint32_t rhs)
{
  return (lhs <= rhs) ? lhs : rhs;
}

static inline uint64_t comm_max_u64(uint64_t lhs, uint64_t rhs)
{
  return (lhs >= rhs) ? lhs : rhs;
}

static inline uint64_t comm_min_u64(uint64_t lhs, uint64_t rhs)
{
  return (lhs <= rhs) ? lhs : rhs;
}


#endif /* MC_IO_COMMUNICATION_BASE_H_ */
