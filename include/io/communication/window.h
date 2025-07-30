#ifndef MC_MESSAGE_WINDOW_H_
#define MC_MESSAGE_WINDOW_H_

#include <stdbool.h>
#include <stdint.h>
#include "alg/span.h"
#include "core/time.h"


enum definitions
{
  INVALID_ID = -1,
  HEADER     = 0xC7E9
};

typedef uint16_t mc_comm_hdr;
typedef uint32_t mc_comm_id;
typedef uint8_t mc_comm_idx;

typedef enum
{
  PKT_DATA = 0,
  PKT_ACK
}pkt_type_t;

typedef struct // TODO(MN): pads
{
  mc_comm_hdr header;
  pkt_type_t  type;
  mc_comm_id  id;
  uint32_t    size;
  char        data[0];
}pkt_t;// TODO(MN): Must be As size as window_size

typedef struct 
{
  mc_time_t sent_time_us;
  bool      is_acked;// TODO(MN): Large pad
  pkt_t     packet;
}wnd_t;


void     wnd_clear(wnd_t* const wnd);
void     wnd_write(wnd_t* const wnd, mc_span buffer, mc_comm_id id);
void*    wnd_get_data(wnd_t* const wnd);
uint32_t wnd_get_data_size(const wnd_t* const wnd);
void     wnd_ack(wnd_t* const wnd);
bool     wnd_is_acked(const wnd_t* const wnd);
bool     wnd_is_valid(const wnd_t* const wnd);
bool     wnd_is_timedout(const wnd_t* const wnd, uint32_t timeout_us);


#endif /* MC_MESSAGE_WINDOW_H_ */
