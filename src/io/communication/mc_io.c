#include <unistd.h>
#include "mc_io.h"


void io_init(mc_io* this, mc_io io)
{
  *this = io;
}

void io_recv(mc_comm* this, io_data_ready_cb data_ready, void* arg)
{
  const uint32_t required_size = this->rcv->pool.window_size - this->rcv->temp_stored;
  void* const temp_buffer = (char*)this->rcv->temp_window + this->rcv->temp_stored;
  const uint32_t read_size = this->io.recv(temp_buffer, required_size);
  
  if (0 != read_size) {
    this->rcv->temp_stored += read_size;
    frame_recv(this->rcv, data_ready, this); 
  }
}

bool send_buffer(mc_comm* this, const void* buffer, uint32_t size)
{
  uint8_t index = 3;
  while (index--) {
    if (size == this->io.send(buffer, size)) {
      return true;
    }// TODO(MN): Handle if send is incomplete. attempt 3 times! 

    usleep(100);
  }
  
  return false;
}
