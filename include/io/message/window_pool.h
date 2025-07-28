/* TODO(MN): This container is not a queue, so doesn't need to have enqueue and dequeue.
 * Use const for all arguments
 */

#ifndef MC_MESSAGE_WINDOW_POOL_H_
#define MC_MESSAGE_WINDOW_POOL_H_

#include "window.h"
#include "io/message/message.h"// TODO(MN): Remove for encapsulation

typedef struct
{
  wnd_t*   windows;
  uint32_t window_size;
  uint32_t data_size;
  id_t     bgn_id;
  id_t     end_id;// TODO(MN): Remove
  idx_t    bgn_index;
  idx_t    capacity;
  pkt_t    temp_window[0];
}wndpool_t;

typedef void (*wndpool_on_done_fn)(const mc_span data, id_t id);


void    wndpool_clear(wndpool_t* const this);
bool    wndpool_is_empty(wndpool_t* const this);
bool    wndpool_is_full(wndpool_t* const this);
bool    wndpool_contains(wndpool_t* const this, id_t id);
wnd_t*  wndpool_get(wndpool_t* const this, id_t id);
uint8_t wndpool_get_count(const wndpool_t* const this);
uint8_t wndpool_get_capacity(const wndpool_t* const this);
void    wndpool_remove_first(wndpool_t* const this);// TODO(MN): Pop top
void    wndpool_remove_acked(wndpool_t* const this, mc_io_receive_cb on_receive);// TODO(MN): Make it private
bool    wndpool_insert(wndpool_t* const this, const mc_span data, const id_t id);
bool    wndpool_push(wndpool_t* const this, const mc_span data);
bool    wndpool_ack(wndpool_t* const this, id_t id, mc_io_receive_cb on_done);


#endif /* MC_MESSAGE_WINDOW_POOL_H_ */
