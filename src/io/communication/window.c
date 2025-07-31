/* TODO(MN): const for all arguments
 * Rename this module to segment
 */
#include <string.h>
#include "io/communication/window.h"


void wnd_clear(wnd_t* const wnd)
{
  wnd->packet.id = INVALID_ID;
  wnd->is_acked  = true;
}

void wnd_write(wnd_t* const wnd, mc_span buffer, mc_comm_id id)
{
  wnd->packet.header = HEADER;
  wnd->packet.type   = PKT_DATA;
  wnd->is_acked      = false;
  wnd->packet.size   = buffer.capacity;
  wnd->packet.id     = id;
  wnd->sent_time_us  = mc_now_u();
  memcpy(wnd->packet.data, buffer.data, buffer.capacity);
}

char* wnd_get_data(wnd_t* const wnd)
{
  return wnd->packet.data;
}

uint32_t wnd_get_data_size(const wnd_t* const wnd)
{
  return wnd->packet.size;
}

void wnd_ack(wnd_t* const wnd)
{
  wnd->is_acked  = true;
}

bool wnd_is_acked(const wnd_t* const wnd)
{
  return wnd->is_acked;
}

bool wnd_is_valid(const wnd_t* const wnd)
{
  return (INVALID_ID != wnd->packet.id);
}

bool wnd_is_timedout(const wnd_t* const wnd, uint32_t timeout_us)
{
  return (mc_now_u() > (wnd->sent_time_us + timeout_us));
}
