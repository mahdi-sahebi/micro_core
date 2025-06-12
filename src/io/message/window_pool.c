#include <stdlib.h>
#include "io/message/window_pool.h"


void wndpool_clear(wndpool_t* const this)
{

}

bool wndpool_is_empty(wndpool_t* const this)
{
  return false;
}

bool wndpool_is_full(wndpool_t* const this)
{
  return false;
}

bool wndpool_contains(wndpool_t* const this, id_t id)
{
  return false;
}

wnd_t* wndpool_get(wndpool_t* const this, id_t id)
{
  return NULL;
}

bool wndpool_enqueue(wndpool_t* const this, const mc_span data)
{
  return true;
}

bool wndpool_dequeue(wndpool_t* const this, const mc_span data)
{
  return false;
}

uint32_t wndpool_get_capacity(wndpool_t* const this)
{
  return 0;
}

id_t wndpool_get_bgn_id(wndpool_t* const this)
{
  return this->begin_window_id;
}

id_t wndpool_get_end_id(wndpool_t* const this)
{
  return this->begin_window_id + this->capacity;
}

bool wndpool_push(wndpool_t* const this, const mc_span data)
{
  return false;
}

bool wndpool_ack(wndpool_t* const this, id_t id, wndpool_on_done_fn on_done)
{
  return false;
}
