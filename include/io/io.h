#ifndef MC_IO_H_
#define MC_IO_H_


#include <stdint.h>


typedef uint32_t (*mc_io_recv_fn)(void* const data, uint32_t size);
typedef uint32_t (*mc_io_send_fn)(const void* const data, uint32_t size);

typedef struct
{
  mc_io_recv_fn recv;
  mc_io_send_fn send;
}mc_io_t;


#define mc_io(RECV, SEND)     ((mc_io_t){.recv = RECV, .send = SEND})


#endif /* MC_IO_H_ */
