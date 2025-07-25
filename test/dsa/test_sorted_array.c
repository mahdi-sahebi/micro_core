#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "core/error.h"
#include "core/version.h"
#include "core/time.h"
#include "dsa/sarray.h"


static int8_t comparator_i16(const void* a, const void* b) 
{
  return *(const int*)a - *(const int*)b;
}

static int test_required_size()
{
  mc_result_u32 result = {0};
  
  result = mc_sarray_required_size(sizeof(char), 7);
  if ((MC_SUCCESS != result.result) || 
      (result.value < (7 * sizeof(char))) ||
      (result.value > (7 * sizeof(char) + 30))) {
    return MC_ERR_RUNTIME;
  }

  result = mc_sarray_required_size(sizeof(int16_t), 10);
  if ((MC_SUCCESS != result.result) || 
      (result.value < (10 * sizeof(int16_t))) ||
      (result.value > (10 * sizeof(int16_t) + 30))) {
    return MC_ERR_RUNTIME;
  }
  
  result = mc_sarray_required_size(sizeof(double), 500);
  if ((MC_SUCCESS != result.result) || 
      (result.value < (500 * sizeof(double))) ||
      (result.value > (500 * sizeof(double) + 30))) {
    return MC_ERR_RUNTIME;
  }
  
  result = mc_sarray_required_size(sizeof(int16_t), 0);
  if (MC_ERR_INVALID_ARGUMENT != result.result) {
    return MC_ERR_INVALID_ARGUMENT;
  }
  
  result = mc_sarray_required_size(0, 5);
  if (MC_ERR_INVALID_ARGUMENT != result.result) {
    return MC_ERR_INVALID_ARGUMENT;
  }
  
  return MC_SUCCESS;
}

static int test_invalid_creation()
{
  char memory[10];
  mc_span buffer = mc_span(memory, sizeof(memory));
  mc_result_ptr result = {0};
  
  result = mc_sarray_init(mc_span(NULL, 10), sizeof(int16_t), 5, comparator_i16);
  if (MC_ERR_INVALID_ARGUMENT != result.result) {
    return MC_ERR_RUNTIME;
  }
  
  result = mc_sarray_init(mc_span(memory, 0), sizeof(int16_t), 5, comparator_i16);
  if (MC_ERR_INVALID_ARGUMENT != result.result) {
    return MC_ERR_RUNTIME;
  }
  
  result = mc_sarray_init(buffer, 0, 5, comparator_i16);
  if (MC_ERR_INVALID_ARGUMENT != result.result) {
    return MC_ERR_RUNTIME;
  }
  
  result = mc_sarray_init(buffer, sizeof(int16_t), 0, comparator_i16);
  if (MC_ERR_INVALID_ARGUMENT != result.result) {
    return MC_ERR_RUNTIME;
  }
  
  result = mc_sarray_init(buffer, sizeof(int16_t), 5, NULL);
  if (MC_ERR_INVALID_ARGUMENT != result.result) {
    return MC_ERR_RUNTIME;
  }

  return MC_SUCCESS;
}


int main()
{
  printf("[MICRO CORE - DSA - SORTED_ARRAY - VERSION]: %u.%u.%u\n", MC_VERSION_MAJOR, MC_VERSION_MINOR, MC_VERSION_PATCH);
  uint32_t total_failed = 0;

  test_required_size();

  printf("[test_required_size]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_required_size();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_invalid_creation]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_invalid_creation();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  if (0 != total_failed) {
    printf("#%d Tests failed\n", total_failed);
  }

  printf("passed\n");
  return MC_SUCCESS;
}
