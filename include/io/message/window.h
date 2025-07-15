#ifndef MC_MESSAGE_WINDOW_H_
#define MC_MESSAGE_WINDOW_H_

#include <stdbool.h>
#include <stdint.h>
#include "dsa/span.h"
#include "core/time.h"


enum definitions
{
  INVALID_ID = -1,
  HEADER     = 0xA5B3C7E9
};

typedef uint32_t id_t;
typedef uint8_t idx_t;

typedef enum 
{
  PKT_DATA = 0,
  PKT_ACK
}pkt_type_t;

typedef struct 
{
  uint32_t   header;
  pkt_type_t type;
  id_t       id;
  uint32_t   size;
  char       data[0];
}pkt_t;// TODO(MN): Must be As size as window_size

typedef struct 
{
  mc_time_t sent_time;// TODO(MN): Add postfix us
  bool  is_acked;
  pkt_t packet;
}wnd_t;


void     wnd_clear(wnd_t* const wnd);
void     wnd_write(wnd_t* const wnd, mc_span buffer, id_t id);
void*    wnd_get_data(wnd_t* const wnd);
uint32_t wnd_get_data_size(const wnd_t* const wnd);
void     wnd_ack(wnd_t* const wnd);
bool     wnd_is_acked(const wnd_t* const wnd);
bool     wnd_is_valid(const wnd_t* const wnd);
bool     wnd_is_timedout(const wnd_t* const wnd, uint32_t timeout_us);


#endif /* MC_MESSAGE_WINDOW_H_ */
