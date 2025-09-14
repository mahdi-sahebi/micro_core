#ifndef MC_IO_COMMUNICATION_H_
#define MC_IO_COMMUNICATION_H_

#include <stdint.h>
#include "core/error.h"
#include "alg/mc_buffer.h"
#include "io/io.h"

typedef struct _mc_comm_t mc_comm;

typedef struct
{
  mc_io    io;
  uint16_t window_size;
  uint16_t window_capacity;
}mc_comm_cfg;


#define mc_comm_cfg(IO, WND_SIZE, WND_CAPACITY)\
	(mc_comm_cfg){.io = (IO), .window_size = (WND_SIZE), .window_capacity = (WND_CAPACITY)}

mc_result_u32  mc_comm_get_alloc_size(mc_comm_cfg config);
mc_result_ptr  mc_comm_init  (mc_buffer alloc_buffer, mc_comm_cfg config);
mc_error       mc_comm_update(mc_comm* this);
mc_result_u32  mc_comm_recv  (mc_comm* this, void* dst_data,       uint32_t size, uint32_t timeout_us);
mc_result_u32  mc_comm_send  (mc_comm* this, const void* src_data, uint32_t size, uint32_t timeout_us);
mc_result_bool mc_comm_flush (mc_comm* this, uint32_t timeout_us);


#endif /* MC_IO_COMMUNICATION_H_ */
