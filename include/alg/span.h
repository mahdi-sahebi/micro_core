/* TODO(MN): sub-span, rename to m buffer, is in range,...
 * Rename namespace _t
 * Change data to char
 * Move into the core?
 */

#ifndef MC_SPAN_H_
#define MC_SPAN_H_

// TODO(MN):is in range,...
// TODO(MN): Without API ?
#include <stdbool.h>
#include <stdint.h>


typedef struct
{
    char*    data;
    uint32_t count;
    uint32_t data_size;
}mc_span;


#define mc_span_raw(DATA, COUNT, DATA_SIZE)     (mc_span){.data = (char*)(DATA), .count = (COUNT), .data_size = (DATA_SIZE)}
#define mc_span(DATA, COUNT)                    mc_span_raw(DATA, COUNT, sizeof(char))
bool    mc_span_is_empty(mc_span buffer);
bool    mc_span_is_null(mc_span buffer);


#endif /* MC_SPAN_H_ */

