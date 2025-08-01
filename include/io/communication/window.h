/* TODO(MN): 
 * Rename this module to segment
 */

#ifndef MC_IO_COMMUNICATION_WINDOW_H_
#define MC_IO_COMMUNICATION_WINDOW_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
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



#define wnd_clear(WND)\
do {\
  (WND)->packet.id = INVALID_ID;\
  (WND)->is_acked  = true;\
} while (0)

#define wnd_write(WND, BUFFER, ID)\
do {\
  (WND)->packet.header = HEADER;\
  (WND)->packet.type   = PKT_DATA;\
  (WND)->is_acked      = false;\
  (WND)->packet.size   = (BUFFER).capacity;\
  (WND)->packet.id     = (ID);\
  (WND)->sent_time_us  = mc_now_u();\
  memcpy((WND)->packet.data, (BUFFER).data, (BUFFER).capacity);\
} while (0)

#define wnd_get_data(WND)\
  (WND)->packet.data

#define wnd_get_data_size(WND)\
  (WND)->packet.size

#define wnd_ack(WND)\
  (WND)->is_acked = true

#define wnd_is_acked(WND)\
  (WND)->is_acked

#define wnd_is_valid(WND)\
  (INVALID_ID != (WND)->packet.id)

#define wnd_is_timedout(WND, TIMEOUT_US)\
  (mc_now_u() > ((WND)->sent_time_us + (TIMEOUT_US)))


#endif /* MC_IO_COMMUNICATION_WINDOW_H_ */
