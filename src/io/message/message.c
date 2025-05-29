/* TODO(MN): Check inputs
 * Remove dynamic allocation
 * Remove standard library dependencies as much as possible
 * Define a network interface with write and read handlers
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "io/message/message.h"


#define DATA_LEN          10
#define WINDOW_LEN        3

enum def
{
  INVALID_ID = -1
};

typedef struct {
  uint32_t id;
  uint32_t size;
  uint32_t data[DATA_LEN];
}packet_t;

typedef struct {
  packet_t packet;
}window_t;

struct _mc_msg_t
{ 
  mc_msg_read_fn  read;
  mc_msg_write_fn write;
  window_t windows[WINDOW_LEN];
  uint32_t begin_window_id;
  uint32_t next_window_id;
  uint32_t begin_index;
  uint32_t end_index;
  uint32_t count;
  uint32_t capacity;
};

mc_msg_t* mc_msg_new(mc_msg_read_fn read_fn, mc_msg_write_fn write_fn, uint32_t window_size, uint32_t window_capacity, mc_msg_on_receive_fn on_receive)
{
  mc_msg_t* msg = malloc(sizeof(mc_msg_t));

  msg->read            = read_fn;
  msg->write           = write_fn;
  msg->begin_window_id = 0;
  msg->next_window_id  = 0;
  msg->begin_index     = 0;
  msg->end_index       = 0;
  msg->count           = 0;
  msg->capacity        = WINDOW_LEN;

  for (uint32_t index = 0; index < msg->capacity; index++) {
    msg->windows[index].packet.id = INVALID_ID;
  }
  
  return msg;
}

void mc_msg_free(mc_msg_t** const msg)
{
  free(*msg);
  *msg = NULL;
}



#undef WINDOW_LEN
#undef DATA_LEN
