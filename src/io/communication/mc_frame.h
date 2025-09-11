#ifndef MC_IO_COMMUNICATION_FRAME_H_
#define MC_IO_COMMUNICATION_FRAME_H_

#include <stdint.h>
#include <stdbool.h>
#include "alg/mc_buffer.h"
#include "io/io.h"
#include "io/communication/window.h"
#include "io/communication/window_pool.h"


 
typedef struct __attribute__((packed))
{
  // TODO(mn): pad
  uint16_t  temp_stored;// TODO(MN): Check max temp size
  mc_pkt*   temp_window;
  wndpool_t pool;
}mc_frame;


void      frame_init(mc_frame* this, uint16_t window_size, uint8_t capacity);
void      frame_recv(mc_frame* this, mc_data_ready_cb data_ready, void* arg);
mc_buffer frame_send(mc_frame* this, mc_buffer buffer, mc_data_ready_cb data_ready, void* arg);


#endif /* MC_IO_COMMUNICATION_FRAME_H_ */
