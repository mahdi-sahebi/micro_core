#include <unistd.h>
#include "mc_frame.h"
#include "mc_io.h"
#include "mc_posix.h"


void io_init(mc_io* this, mc_io io)
{
  *this = io;
}

void io_recv(mc_comm* this, io_cb_data_ready data_ready, void* arg)
{
  (void)arg;// the receive context is this, supplied to frame_recv directly
  cuint32_t required_size = (uint32_t)this->rcv->pool.window_size - (uint32_t)this->rcv->temp_stored;
  void* const temp_buffer = (char*)(this->rcv->temp_window) + this->rcv->temp_stored;
  cuint32_t read_size = this->io.recv(temp_buffer, required_size);

  if (0U != read_size) {
    this->rcv->temp_stored = (uint16_t)(this->rcv->temp_stored + read_size);
    frame_recv(this->rcv, data_ready, this);
  }
}

bool io_send(mc_comm* this, cvoid* buffer, uint32_t size)
{
  bool sent = false;
  for (uint8_t attempt = 0U; (attempt < 3U) && (!sent); attempt++) {
    cuint32_t sent_size = this->io.send(buffer, size);
    if (sent_size == size) {// TODO(MN): Handle if send is incomplete
      sent = true;
    } else {
      (void)usleep(100);
    }
  }

  return sent;
}
