#ifndef MC_IO_COMMUNICATION_H_
#define MC_IO_COMMUNICATION_H_

#include <stdint.h>
#include "core/error.h"
#include "alg/mc_buffer.h"
#include "io/io.h"

typedef struct _mc_comm_t mc_comm;

typedef struct
{
  uint16_t size;
  uint8_t  capacity;// TOOD(MN): Rename to cap?
}mc_comm_wnd;

#define mc_comm_wnd(SIZE, CAPACITY)\
  (mc_comm_wnd){.size = (SIZE), .capacity = (CAPACITY)}

typedef struct
{
  mc_io       io;
  mc_comm_wnd recv;
  mc_comm_wnd send;
}mc_comm_cfg;

#define mc_comm_cfg(IO, RECV_WND, SEND_WND)\
  (mc_comm_cfg){.io = (IO), .recv = (RECV_WND), .send = (SEND_WND)}


mc_u32  mc_comm_get_alloc_size(mc_comm_cfg config);
mc_ptr  mc_comm_init  (mc_buffer alloc_buffer, mc_comm_cfg config);
mc_err  mc_comm_update(mc_comm* this);
mc_u32  mc_comm_recv  (mc_comm* this, void* dst_data,       uint32_t size, uint32_t timeout_us);
mc_u32  mc_comm_send  (mc_comm* this, cvoid* src_data, uint32_t size, uint32_t timeout_us);
mc_bool mc_comm_flush (mc_comm* this, uint32_t timeout_us);


#endif /* MC_IO_COMMUNICATION_H_ */
