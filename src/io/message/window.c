#include <string.h>
#include "io/message/window.h"


void wnd_clear(wnd_t* const wnd)
{
  wnd->packet.id = INVALID_ID;
  wnd->is_acked  = true;
}

void wnd_write(wnd_t* const wnd, const void* const data, uint32_t size, id_t id)
{
  wnd->packet.header  = HEADER;
  wnd->packet.type    = PKT_DATA;
  wnd->is_acked       = false;
  wnd->packet.size    = size;
  wnd->packet.id      = id;
  wnd->send_count     = 0;
  memcpy(wnd->packet.data, data, size);
}

void wnd_ack(wnd_t* const wnd)
{
  wnd->is_acked  = true;
}

bool wnd_is_acked(wnd_t* const wnd)
{
  return wnd->is_acked;
}
