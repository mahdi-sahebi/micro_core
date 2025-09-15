#ifndef MC_IO_COMMUNICATION_BASE_H_
#define MC_IO_COMMUNICATION_BASE_H_

#include <stdint.h>
#include "io/communication/communication.h"
// #include "mc_io.h"
#include "mc_frame.h"


struct _mc_comm_t
{ 
  mc_frame* rcv;// TODO(MN): Use array to reduce one pointer size
  mc_frame* snd;
  mc_io     io;
  uint32_t  send_delay_us;// TODO(MN): Use u16 with 100X us resolution
};


#endif /* MC_IO_COMMUNICATION_BASE_H_ */
