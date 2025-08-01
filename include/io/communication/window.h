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

typedef uint16_t mc_pkt_hdr;
typedef uint32_t mc_pkt_id;// TODO(MN): Handle overflow and decrese the size

typedef enum
{
  PKT_DATA = 0,
  PKT_ACK
}mc_pkt_type;

typedef struct // TODO(MN): pads
{
  mc_pkt_hdr  header;
  mc_pkt_type type;
  mc_pkt_id   id;
  uint32_t    size;
  char        data[0];
}mc_pkt;// TODO(MN): Must be As size as window_size

typedef struct 
{
  mc_time_t sent_time_us;
  bool      is_acked;// TODO(MN): Large pad
  mc_pkt    packet;
}wnd_t;


void     wnd_clear(wnd_t* const wnd);
void     wnd_write(wnd_t* const wnd, mc_span buffer, mc_pkt_id id);
char*    wnd_get_data(wnd_t* const wnd);
uint32_t wnd_get_data_size(const wnd_t* const wnd);
void     wnd_ack(wnd_t* const wnd);
bool     wnd_is_acked(const wnd_t* const wnd);
bool     wnd_is_valid(const wnd_t* const wnd);
bool     wnd_is_timedout(const wnd_t* const wnd, uint32_t timeout_us);


#endif /* MC_MESSAGE_WINDOW_H_ */
