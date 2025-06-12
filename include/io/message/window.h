#ifndef MC_MESSAGE_WINDOW_H_
#define MC_MESSAGE_WINDOW_H_

#include <stdbool.h>
#include <stdint.h>


enum definitions
{
  INVALID_ID = -1,
  HEADER     = 0xA5B3C7E9
};

typedef uint32_t id_t;

typedef enum {
  PKT_DATA = 0,
  PKT_ACK
}pkt_type_t;

typedef struct {
  uint32_t   header;
  pkt_type_t type;
  id_t       id;
  uint32_t   size;
  char       data[0];
}pkt_t;// TODO(MN): As size as window_size

typedef struct {
  uint16_t send_count;
  bool     is_acked;
  pkt_t    packet;
}wnd_t;


void wnd_clear(wnd_t* const wnd);
void wnd_write(wnd_t* const wnd, const void* const data, uint32_t size, id_t id);
void wnd_ack(wnd_t* const wnd);
bool wnd_is_acked(wnd_t* const wnd);


#endif /* MC_MESSAGE_WINDOW_H_ */
