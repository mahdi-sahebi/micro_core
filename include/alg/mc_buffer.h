#ifndef MC_ALG_BUFFER_H_
#define MC_ALG_BUFFER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


typedef struct
{
    char*     data;
    uint32_t  capacity;
    uint32_t  data_size;
} mc_buffer;


static inline mc_buffer mc_buffer_make(void* address, uint32_t size, uint32_t data_size)
{
  mc_buffer buf = {
    .data      = address,
    .data_size = data_size
  };

  if (data_size == 0U) {
    buf.capacity = 0U;
  } else {
    buf.capacity = size / data_size;
  }

  return buf;
}

static inline mc_buffer mc_buffer_char(void* address, uint32_t size)
{
  return mc_buffer_make(address, size, 1U);
}

static inline uint32_t mc_buffer_get_size(const mc_buffer buffer)
{
    return buffer.capacity * buffer.data_size;
}

static inline bool mc_buffer_is_empty(const mc_buffer buffer)
{
  return (buffer.capacity == 0U);
}

static inline bool mc_buffer_is_null(const mc_buffer buffer)
{
  return (buffer.data == NULL);
}

static inline char* mc_buffer_bgn(const mc_buffer buffer)
{
  return buffer.data;
}

static inline char* mc_buffer_end(const mc_buffer buffer)
{
  return &buffer.data[mc_buffer_get_size(buffer)];
}

static inline void* mc_buffer_at(const mc_buffer buffer, uint32_t index)
{
  return &buffer.data[index * buffer.data_size];
}

static inline mc_buffer mc_buffer_null(void)
{
  return mc_buffer_make(NULL, 0U, 1U);
}

#endif /* MC_ALG_BUFFER_H_ */
