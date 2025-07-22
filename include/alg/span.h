#ifndef MC_SPAN_H_
#define MC_SPAN_H_

// TODO(MN): sub-span, rename to m buffer, is in range,...
// TODO(MN): Without API ?
#include <stdbool.h>
#include <stdint.h>


typedef struct
{
    void*    data;
    uint32_t size;// u32? opt. rename to capacity
}mc_span;// TODO(MN): Rename to mc_buffer. add data_size. get_count()


#define mc_span(DATA, SIZE)                 (mc_span){.data = (DATA), .size = (SIZE)}
bool    mc_span_is_empty(mc_span buffer);
bool    mc_span_is_null(mc_span buffer);


#endif /* MC_SPAN_H_ */

