#include <stdlib.h>
#include <stdio.h>
#include "core/error.h"
#include "core/version.h"


static int span_create()
{

    return MC_SUCCESS;
}

static int span_is_null()
{

    return MC_SUCCESS;
}

static int span_is_empty()
{

    return MC_SUCCESS;
}

static int span_fill()
{

    return MC_SUCCESS;
}


int main()
{
    printf("[MICRO CORE VERSION]: %u.%u.%u\n", MC_VERSION_MAJOR, MC_VERSION_MINOR, MC_VERSION_PATCH);

    mc_result result = MC_SUCCESS;

    result = span_create();
    if (MC_SUCCESS != result) {
      printf("[FAILED]: span_create - %u\n", result);
      return result;
    }
    
    result = span_is_null();
    if (MC_SUCCESS != result) {
        printf("[FAILED]: span_is_null - %u\n", result);
        return result;
    }
    
    result = span_is_empty();
    if (MC_SUCCESS != result) {
        printf("[FAILED]: span_is_empty - %u\n", result);
        return result;
    }
    
    result = span_fill();
    if (MC_SUCCESS != result) {
        printf("[FAILED]: span_fill - %u\n", result);
        return result;
    }
    
    return MC_SUCCESS; 
}