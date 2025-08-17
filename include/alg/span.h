#ifndef MC_SPAN_H_
#define MC_SPAN_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


typedef struct
{
  char*    data;
  uint32_t capacity;
  uint32_t data_size;
}mc_span;


#define mc_span_raw(ADDRESS, SIZE, DATA_SIZE)   (mc_span){.data = (char*)(ADDRESS), .capacity = (0 == (DATA_SIZE)) ? 0 : ((SIZE) / (DATA_SIZE)), .data_size = (DATA_SIZE)}
#define mc_span(ADDRESS, SIZE)                  mc_span_raw(ADDRESS, SIZE, sizeof(char))
#define mc_span_is_empty(BUFFER)                (0 == (BUFFER).capacity)
#define mc_span_is_null(BUFFER)                 (NULL == (BUFFER).data)
#define mc_span_get_size(BUFFER)                ((BUFFER).capacity * (BUFFER).data_size)
#define mc_span_bgn(BUFFER)                     ((BUFFER).data)
#define mc_span_end(BUFFER)                     ((BUFFER).data + mc_span_get_size(BUFFER))
#define mc_span_null()                          mc_span(NULL, 0)
// TODO(NM): At


#endif /* MC_SPAN_H_ */

