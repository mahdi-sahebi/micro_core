/* TODO(MN): This container is not a queue, so doesn't need to have enqueue and dequeue.
 * Use const for all arguments
 */

#ifndef MC_MESSAGE_WINDOW_POOL_H_
#define MC_MESSAGE_WINDOW_POOL_H_

#include "window.h"
#include "io/communication/communication.h"// TODO(MN): Remove for encapsulation


typedef uint8_t mc_wnd_idx;

typedef struct
{
  wnd_t*     windows;
  uint32_t   window_size;// TODO(MN): u16
  uint32_t   data_size;// TODO(MN): ?min size
  mc_pkt_id  bgn_id;
  mc_pkt_id  end_id;// TODO(MN): Remove
  uint16_t   last_read_size;
  mc_wnd_idx bgn_index;
  mc_wnd_idx capacity;
  mc_pkt     temp_window[0];
}wndpool_t;

typedef void (*wndpool_on_done_fn)(const mc_span data, mc_pkt_id id);

// TODO(MN): init API
// TODO(MN): Remove unnecessary const
void     wndpool_clear(wndpool_t* const this);
bool     wndpool_is_empty(wndpool_t* const this);
bool     wndpool_is_full(wndpool_t* const this);
bool     wndpool_contains(wndpool_t* const this, mc_pkt_id id);
wnd_t*   wndpool_get(wndpool_t* const this, mc_pkt_id id);
uint8_t  wndpool_get_count(const wndpool_t* const this);
uint8_t  wndpool_get_capacity(const wndpool_t* const this);
uint32_t wndpool_pop(wndpool_t* const this, void* data, uint32_t size);
bool     wndpool_insert(wndpool_t* const this, mc_span data, mc_pkt_id id);
bool     wndpool_push(wndpool_t* const this, const mc_span data);
bool     wndpool_ack(wndpool_t* const this, mc_pkt_id id, mc_io_receive_cb on_done);


#endif /* MC_MESSAGE_WINDOW_POOL_H_ */
