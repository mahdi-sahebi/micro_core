#ifndef MC_IO_H_
#define MC_IO_H_

#include <stdint.h>


typedef uint32_t (*mc_io_recv_fn)(void* const data, uint32_t size);
typedef uint32_t (*mc_io_send_fn)(const void* const data, uint32_t size);

typedef void (*mc_io_receive_cb)(const void* const data, uint32_t size);

typedef struct
{
  mc_io_recv_fn recv;
  mc_io_send_fn send;
}mc_io;


#define mc_io(RECV, SEND)     ((mc_io){.recv = RECV, .send = SEND})


#endif /* MC_IO_H_ */
