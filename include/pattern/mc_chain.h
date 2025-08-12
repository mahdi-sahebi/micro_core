#ifndef MC_PATTERN_CHAIN_H_
#define MC_PATTERN_CHAIN_H_

#include <core/error.h>
#include <alg/span.h>


typedef struct
{
  void*    arg;
  mc_span  buffer;
  mc_error error;
}mc_chain_data;

// typedef mc_chain_data (*mc_chain_
typedef struct
{
  uint8_t 
}mc_chain

#define mc_chain_data(THIS, BUFFER, ERROR)\
  (mc_chain_data){.this = (THIS), .buffer = (BUFFER), .error = (ERROR)}

#define mc_chain_return_on_error(DATA)\
  do {\
    if (MC_SUCCESS != (DATA).error) {\
      return (DATA);\
    }\
  } while (0)


#endif /* MC_PATTERN_CHAIN_H_ */
