#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "core/error.h"
#include "core/version.h"
#include "dsa/span.h"


static int span_create()
{
    mc_span span = mc_span(NULL, 0);

    if (&span == NULL) {
        return MC_ERR_BAD_ALLOC;
    }

    return MC_SUCCESS;
}

static int span_is_null()
{
    mc_span span = mc_span(NULL, 0);

    if (!mc_span_is_null(span)) {
        return MC_ERR_BAD_ALLOC;
    }

    return MC_SUCCESS;
}

static int span_is_empty()
{
    mc_span span = mc_span(NULL, 0);

    if (!mc_span_is_empty(span)) {
        return MC_ERR_BAD_ALLOC;
    }

    return MC_SUCCESS;
}

static int span_fill_1()
{
#define START_INDEX     10
#define SIZE            100
#define CLEAR_MASK      (char)0x5a
#define MASK            (char)0xa5
    char buffer[SIZE];
    memset(buffer, CLEAR_MASK, sizeof(buffer));

    mc_span span = mc_span(buffer + START_INDEX, 20);

    if (mc_span_is_null(span)) {
        return MC_ERR_BAD_ALLOC;
    }

    if (mc_span_is_empty(span)) {
        return MC_ERR_BAD_ALLOC;
    }

    /* Fill mask */
    for (uint32_t index = 0; index < span.size; index++) {
        ((char*)span.data)[index] = MASK;
    }

    /* Verify Ranges */
    for (uint32_t index = 0; index < SIZE; index++) {
        if (index < START_INDEX) { 
            if (CLEAR_MASK != buffer[index]) {
                return MC_ERR_OUT_OF_RANGE;
            }
        } else if ((START_INDEX <= index) && (index < START_INDEX + span.size)) {
            if (MASK != buffer[index]) {
                return MC_ERR_OUT_OF_RANGE;
            }
        } else {
            if (CLEAR_MASK != buffer[index]) {
                return MC_ERR_OUT_OF_RANGE;
            }
        }
    }

    return MC_SUCCESS;
#undef MASK
#undef START_INDEX
#undef SIZE
}

static int span_fill_2()
{
#define START_INDEX     2
#define SIZE            10
#define CLEAR_MASK      (char)0x5a
#define MASK            (char)0xa5
    char buffer[SIZE];
    memset(buffer, CLEAR_MASK, sizeof(buffer));

    mc_span span = mc_span(buffer + START_INDEX, 7);

    if (mc_span_is_null(span)) {
        return MC_ERR_BAD_ALLOC;
    }

    if (mc_span_is_empty(span)) {
        return MC_ERR_BAD_ALLOC;
    }

    /* Fill mask */
    for (uint32_t index = 0; index < span.size; index++) {
        ((char*)span.data)[index] = MASK;
    }

    /* Verify Ranges */
    for (uint32_t index = 0; index < SIZE; index++) {
        if (index < START_INDEX) { 
            if (CLEAR_MASK != buffer[index]) {
                return MC_ERR_OUT_OF_RANGE;
            }
        } else if ((START_INDEX <= index) && (index < START_INDEX + span.size)) {
            if (MASK != buffer[index]) {
                return MC_ERR_OUT_OF_RANGE;
            }
        } else {
            if (CLEAR_MASK != buffer[index]) {
                return MC_ERR_OUT_OF_RANGE;
            }
        }
    }

    return MC_SUCCESS;
#undef MASK
#undef START_INDEX
#undef SIZE
}

int main()
{
    printf("[MICRO CORE - DSA - SPAN - VERSION]: %u.%u.%u\n", MC_VERSION_MAJOR, MC_VERSION_MINOR, MC_VERSION_PATCH);

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
    
    result = span_fill_1();
    if (MC_SUCCESS != result) {
        printf("[FAILED]: span_fill_1 - %u\n", result);
        return result;
    }
    
    result = span_fill_2();
    if (MC_SUCCESS != result) {
        printf("[FAILED]: span_fill_2 - %u\n", result);
        return result;
    }
    
   printf("passed\n");
   return MC_SUCCESS; 
}