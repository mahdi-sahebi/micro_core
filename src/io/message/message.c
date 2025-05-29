
#include <stdlib.h>
#include <stdlib.h>
#include "io/message/message.h"


struct _mc_msg_t
{
    int __pad;
};

mc_msg_t* mc_msg_new(mc_msg_read_fn read_fn, mc_msg_write_fn write_fn, uint32_t window_size, uint32_t window_capacity, mc_msg_on_receive_fn on_receive)
{
  return NULL;
}

void mc_msg_free(mc_msg_t* const msg)
{
}

uint32_t mc_msg_read(mc_msg_t* const msg)
{
  return 0;
}

uint32_t mc_msg_read_finish(mc_msg_t* const msg, uint32_t timeout_us)
{
  return 0;
}

uint32_t mc_msg_write(mc_msg_t* const msg, void* data, uint32_t size)
{
  return 0;
}

uint32_t mc_msg_write_finish(mc_msg_t* const msg, uint32_t timeout_us)
{
  return 0;
}
