/* TODO(MN): sub-span, rename to m buffer, is in range,...
 * Rename namespace _t
 * Change data to char
 */

#ifndef MC_SPAN_H_
#define MC_SPAN_H_

#include <stdbool.h>
#include <stdint.h>


typedef struct
{
  void*    data;
  uint32_t size;// u32? opt
}mc_span;


#define mc_span(DATA, SIZE)                 (mc_span){.data = (DATA), .size = (SIZE)}
bool    mc_span_is_empty(mc_span buffer);
bool    mc_span_is_null(mc_span buffer);


#endif /* MC_SPAN_H_ */

