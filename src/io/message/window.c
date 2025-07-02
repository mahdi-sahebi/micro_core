/* TODO(MN): const for all arguments
 */
#include <string.h>
#include "io/message/window.h"


void wnd_clear(wnd_t* const wnd)
{
  wnd->packet.id = INVALID_ID;
  wnd->is_acked  = true;
}

void wnd_write(wnd_t* const wnd, mc_span buffer, id_t id)
{
  wnd->packet.header = HEADER;
  wnd->packet.type   = PKT_DATA;
  wnd->is_acked      = false;
  wnd->packet.size   = buffer.size;
  wnd->packet.id     = id;
  wnd->send_count    = 0;
  memcpy(wnd->packet.data, buffer.data, buffer.size);
}

void* wnd_get_data(wnd_t* const wnd)
{
  return wnd->packet.data;
}

void wnd_ack(wnd_t* const wnd)
{
  wnd->is_acked  = true;
}

bool wnd_is_acked(wnd_t* const wnd)
{
  return wnd->is_acked;
}

bool wnd_is_valid(const wnd_t* const wnd)
{
  return (INVALID_ID != wnd->packet.id);
}
