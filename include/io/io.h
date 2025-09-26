#ifndef MC_IO_H_
#define MC_IO_H_

#include <stdint.h>
#include "alg/mc_buffer.h"


typedef uint32_t (*mc_io_recv_fn)(void* const data, uint32_t size);
typedef uint32_t (*mc_io_send_fn)(const void* const data, uint32_t size);
typedef void (*mc_io_receive_cb)(const void* const data, uint32_t size);
// TODO(MN): Update arg in the IO base. merge
typedef void (*mc_data_ready_cb)(const mc_buffer buffer, void* arg);

typedef struct
{
  mc_io_recv_fn recv;
  mc_io_send_fn send;
}mc_io;


#define mc_io(RECV, SEND)     ((mc_io){.recv = (RECV), .send = (SEND)})


#endif /* MC_IO_H_ */
