#include <stdlib.h>
#include "alg/span.h"

bool mc_span_is_empty(mc_span buffer)
{
    return (0 == buffer.count);
}

bool mc_span_is_null(mc_span buffer)
{
    return (NULL == buffer.data);
}

