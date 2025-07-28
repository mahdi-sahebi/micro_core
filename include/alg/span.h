#ifndef MC_SPAN_H_
#define MC_SPAN_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


typedef struct
{
  char*    data;
  uint32_t count;
  uint32_t data_size;
}mc_span;


#define mc_span_raw(DATA, COUNT, DATA_SIZE)     (mc_span){.data = (char*)(DATA), .count = (COUNT), .data_size = (DATA_SIZE)}
#define mc_span(DATA, COUNT)                    mc_span_raw(DATA, COUNT, sizeof(char))
#define mc_span_is_empty(SPAN)                  (0 == (SPAN).count)
#define mc_span_is_null(SPAN)                   (NULL == (SPAN).data)


#endif /* MC_SPAN_H_ */

