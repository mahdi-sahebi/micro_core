/* TODO(MN): This container is not a queue, so doesn't need to have enqueue and dequeue.
 * Use const for all arguments
 */

#ifndef MC_MESSAGE_WINDOW_POOL_H_
#define MC_MESSAGE_WINDOW_POOL_H_

#include "window.h"


typedef struct
{
  uint32_t bgn_id;
  uint32_t end_id;
  uint32_t bgn_index;
  uint32_t window_size;
  uint32_t data_size;
  uint32_t capacity;
  wnd_t*   windows;
  char     temp_window[0];
}wndpool_t;

typedef void (*wndpool_on_done_fn)(const mc_span data, id_t id);


void     wndpool_clear(wndpool_t* const this);
bool     wndpool_is_empty(wndpool_t* const this);
bool     wndpool_is_full(wndpool_t* const this);
bool     wndpool_contains(wndpool_t* const this, id_t id);
wnd_t*   wndpool_get(wndpool_t* const this, id_t id);
uint32_t wndpool_get_count(wndpool_t* const this);
uint32_t wndpool_get_capacity(wndpool_t* const this);
bool     wndpool_enqueue(wndpool_t* const this, const mc_span data);
bool     wndpool_dequeue(wndpool_t* const this, const mc_span data);
void     wndpool_remove_first(wndpool_t* const this);// TODO(MN): Pop top
bool     wndpool_insert(wndpool_t* const this, const mc_span data, const id_t id);
bool     wndpool_push(wndpool_t* const this, const mc_span data);
bool     wndpool_ack(wndpool_t* const this, id_t id, wndpool_on_done_fn on_done);


#endif /* MC_MESSAGE_WINDOW_POOL_H_ */
