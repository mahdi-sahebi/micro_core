#ifndef MC_ALG_BUFFER_H_
#define MC_ALG_BUFFER_H_

#include <stdint.h>
#include <stdlib.h>


typedef struct
{
  char*    data;
  uint32_t capacity;// TODO(MN): rename to size
  uint32_t data_size;// TODO(MN): Remove or extend to array
}mc_buffer;


#define mc_buffer_raw(ADDRESS, SIZE, DATA_SIZE)   (mc_buffer){.data = (char*)(ADDRESS), .capacity = (0 == (DATA_SIZE)) ? 0 : ((SIZE) / (DATA_SIZE)), .data_size = (DATA_SIZE)}
#define mc_buffer(ADDRESS, SIZE)                  mc_buffer_raw(ADDRESS, SIZE, sizeof(char))
#define mc_buffer_is_empty(BUFFER)                (0 == (BUFFER).capacity)
#define mc_buffer_is_null(BUFFER)                 (NULL == (BUFFER).data)
#define mc_buffer_get_size(BUFFER)                ((BUFFER).capacity * (BUFFER).data_size)
#define mc_buffer_bgn(BUFFER)                     ((BUFFER).data)
#define mc_buffer_end(BUFFER)                     ((BUFFER).data + mc_buffer_get_size(BUFFER))
#define mc_buffer_null()                          mc_buffer(NULL, 0)


#endif /* MC_ALG_BUFFER_H_ */

