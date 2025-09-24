/* TODO(MN): 
 * Rename this module to segment
 */

#ifndef MC_IO_COMMUNICATION_WINDOW_H_
#define MC_IO_COMMUNICATION_WINDOW_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "alg/mc_buffer.h"
#include "core/time.h"


enum definitions
{
  INVALID_ID = -1,
  HEADER     = 0xC7E9
};

typedef uint16_t mc_pkt_hdr;
typedef uint32_t mc_pkt_id;// TODO(MN): Reduce size. Handle overflow and decrese the size

typedef enum __attribute__((packed))
{
  PKT_DATA = 0,
  PKT_ACK 
}mc_pkt_type;

typedef struct __attribute__((packed))// TODO(MN): Pads
{
  mc_pkt_hdr  header;// TODO(NM): Rename to hdr
  uint16_t    crc;
  mc_pkt_type type;// TODO(MN) : 1;
  mc_pkt_id   id;
  uint32_t    size;// TODO(MN): Reduce size
  char        data[0];
}mc_pkt;// TODO(MN): Must be As size as window_size

typedef struct __attribute__((packed))// TODO(MN): Pads
{
  mc_time_t sent_time_us;
  bool      is_acked;// TODO(MN): 1 bit
bool is_sent;// TODO(MN): Rename
  mc_pkt    packet;
}wnd_t;



// TODO(MN): Rename to wnd_node_size
#define wnd_get_size(WINDOW_SIZE)\
  ((WINDOW_SIZE) + (sizeof(wnd_t) - sizeof(mc_pkt)))
// TODO(MN): Use in some places
#define wnd_get_payload_size(WINDOW_SIZE)\
  ((WINDOW_SIZE) - sizeof(mc_pkt))

#define wnd_clear(WND)\
do {\
  (WND)->packet.id = INVALID_ID;\
  (WND)->is_acked  = true;\
  (WND)->packet.size = 0;\
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


#endif /* MC_IO_COMMUNICATION_WINDOW_H_ */
