#ifndef MC_IO_H_
#define MC_IO_H_

#include <stdint.h>
#include "alg/mc_buffer.h"


typedef uint32_t (*mc_io_fn_recv)(void* const data, uint32_t size);
typedef uint32_t (*mc_io_fn_send)(const void* const data, uint32_t size);
typedef void (*mc_io_cb_receive)(const void* const data, uint32_t size);
// TODO(MN): Update arg in the IO base. merge
typedef void (*mc_cb_data_ready)(const mc_buffer buffer, void* arg);

typedef struct
{
  mc_io_fn_recv recv;
  mc_io_fn_send send;
}mc_io;


#define mc_io(RECV, SEND)     ((mc_io){.recv = (RECV), .send = (SEND)})


#endif /* MC_IO_H_ */
