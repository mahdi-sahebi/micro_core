/* TODO(MN): This container is not a queue, so doesn't need to have enqueue and dequeue.
 * Use const for all arguments
 */

#ifndef MC_MESSAGE_WINDOW_POOL_H_
#define MC_MESSAGE_WINDOW_POOL_H_

#include "core/time.h"
#include "alg/algorithm.h"
#include "window.h"


typedef uint8_t mc_wnd_idx;

typedef struct __attribute__((packed))
{
  mc_time_t  update_time;
  mc_pkt_id  bgn_id;// TODO(MN): Handle overflow
  mc_pkt_id  end_id;// TODO(MN): Remove
  uint16_t   window_size;
  uint16_t   stored_size;// TODO(MN): Use for sender. Remove it. use packet.size. rename to last_window_stored
  mc_wnd_idx bgn_index;
  mc_wnd_idx capacity;
  wnd_t      windows[0];
}wndpool_t;

typedef void (*wndpool_on_done_fn)(mc_buffer buffer, void* arg);

#define WNDPOOL_GET_WINDOWS_SIZE(WINDOW_SIZE, CAPACITY)\
  ((CAPACITY) * wnd_get_size(WINDOW_SIZE))


void     wndpool_init(wndpool_t* this, uint16_t window_size, uint8_t capacity);
void     wndpool_clear(wndpool_t* this);
bool     wndpool_contains(const wndpool_t* this, mc_pkt_id id);
wnd_t*   wndpool_get(wndpool_t* this, mc_pkt_id id);
uint8_t  wndpool_get_count(const wndpool_t* this);
uint8_t  wndpool_get_capacity(const wndpool_t* this);
bool     wndpool_update(wndpool_t* this, mc_buffer buffer, mc_pkt_id id);
bool     wndpool_ack(wndpool_t* this, mc_pkt_id id);
uint32_t wndpool_read(wndpool_t* this, mc_buffer buffer);
uint32_t wndpool_write(wndpool_t* this, mc_buffer buffer, wndpool_on_done_fn on_done, void* arg);
bool     wndpool_is_empty(const wndpool_t* this);
void     wndpool_update_header(wndpool_t* this);


#endif /* MC_MESSAGE_WINDOW_POOL_H_ */
