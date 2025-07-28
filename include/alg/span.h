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


#define mc_span_raw(DATA, COUNT, DATA_SIZE)     (mc_span){.data = (char*)(DATA), .capacity = (COUNT), .data_size = (DATA_SIZE)}
#define mc_span(DATA, COUNT)                    mc_span_raw(DATA, COUNT, sizeof(char))
#define mc_span_is_empty(SPAN)                  (0 == (SPAN).capacity)
#define mc_span_is_null(SPAN)                   (NULL == (SPAN).data)
#define mc_span_get_size(SPAN)                  ((SPAN).capacity * (SPAN).data_size)

#endif /* MC_SPAN_H_ */

