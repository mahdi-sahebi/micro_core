#include <stdint.h>
#include <string.h>
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

static int test_correct_creation_i16()
{
  char memory[20];
  mc_result_ptr result_ptr = {0};
  mc_result_u32 result_u32 = {0};
  
  result_ptr = mc_sarray_init(mc_span(memory, sizeof(memory)), sizeof(int16_t), 10, comparator_i16);
  mc_sarray array = result_ptr.data;
  if ((MC_SUCCESS != result_ptr.result) || (NULL == array)) {
    return MC_ERR_BAD_ALLOC;
  }

  result_u32 = mc_sarray_get_capacity(array);
  if ((MC_SUCCESS != result_u32.result) || (10 != result_u32.value)) {
    return MC_ERR_BAD_ALLOC;
  }
  
  result_u32 = mc_sarray_get_count(array);
  if ((MC_SUCCESS != result_u32.result) || (0 != result_u32.value)) {
    return MC_ERR_BAD_ALLOC;
  }

  result_u32 = mc_sarray_get_data_size(array);
  if ((MC_SUCCESS != result_u32.result) || (sizeof(int16_t) != result_u32.value)) {
    return MC_ERR_BAD_ALLOC;
  }
  
  return MC_SUCCESS;
}

static int8_t comparator_str(const void* a, const void* b) 
{
  return strcmp(a, b);
}

static int test_correct_creation_str()
{
  char memory[10];
  mc_result_ptr result_ptr = {0};
  mc_result_u32 result_u32 = {0};
  
  result_ptr = mc_sarray_init(mc_span(memory, sizeof(memory)), sizeof(memory), 1, comparator_str);
  mc_sarray array = result_ptr.data;
  if ((MC_SUCCESS != result_ptr.result) || (NULL == array)) {
    return MC_ERR_BAD_ALLOC;
  }

  result_u32 = mc_sarray_get_capacity(array);
  if ((MC_SUCCESS != result_u32.result) || (1 != result_u32.value)) {
    return MC_ERR_BAD_ALLOC;
  }
  
  result_u32 = mc_sarray_get_count(array);
  if ((MC_SUCCESS != result_u32.result) || (0 != result_u32.value)) {
    return MC_ERR_BAD_ALLOC;
  }

  result_u32 = mc_sarray_get_data_size(array);
  if ((MC_SUCCESS != result_u32.result) || (sizeof(memory) != result_u32.value)) {
    return MC_ERR_BAD_ALLOC;
  }
  
  return MC_SUCCESS;
}

static int test_single_data()
{
  int16_t memory[10];
  mc_span buffer = mc_span(memory, sizeof(memory));
  mc_result_bool result_bool = {0};
  mc_result_ptr result_ptr = {0};
  
  mc_sarray array = mc_sarray_init(buffer, sizeof(int16_t), 10, comparator_i16).data;
  
  result_bool = mc_sarray_is_empty(array);
  if ((MC_SUCCESS != result_bool.result) || (false == result_bool.value)) {
    return MC_ERR_RUNTIME;
  }
  
  int16_t value = 42;
  mc_result result = mc_sarray_insert(array, &value);
  if (MC_SUCCESS != result) {
    return result;
  }
  
  result_bool = mc_sarray_is_empty(array);
  if ((MC_SUCCESS != result_bool.result) || (true == result_bool.value)) {
    return MC_ERR_RUNTIME;
  }
  
  mc_result_u32 result_u32 = mc_sarray_get_count(array);
  if ((MC_SUCCESS != result_u32.result) || (1 != result_u32.value)) {
    return MC_ERR_RUNTIME;
  }
  
  result_ptr = mc_sarray_get(array, 0);
  if ((MC_SUCCESS != result_ptr.result) || (NULL == result_ptr.data)) {
    return MC_ERR_OUT_OF_RANGE;
  }
  const int16_t read_value = *(int16_t*)result_ptr.data;
  if (value != read_value) {
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

  printf("[test_correct_creation_i16]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_correct_creation_i16();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_correct_creation_str]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_correct_creation_str();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_single_data]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_single_data();
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
