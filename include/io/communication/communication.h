#ifndef MC_IO_COMMUNICATION_H_
#define MC_IO_COMMUNICATION_H_

#include <stdint.h>
#include "core/error.h"
#include "alg/span.h"
#include "io/io.h"

typedef struct _mc_comm_t mc_comm;


mc_result_u32  mc_comm_get_alloc_size(uint16_t window_size, uint8_t windows_capacity);
mc_result_ptr  mc_comm_init  (mc_span alloc_buffer, uint16_t window_size, uint8_t windows_capacity, mc_io io);
mc_error       mc_comm_update(mc_comm* this);
mc_result_u32  mc_comm_recv  (mc_comm* this, void* dst_data,       uint32_t size, uint32_t timeout_us);
mc_result_u32  mc_comm_send  (mc_comm* this, const void* src_data, uint32_t size, uint32_t timeout_us);
mc_result_bool mc_comm_flush (mc_comm* this, uint32_t timeout_us);


#endif /* MC_IO_COMMUNICATION_H_ */
