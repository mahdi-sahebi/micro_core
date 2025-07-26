#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "core/error.h"
#include "core/version.h"
#include "core/time.h"
#include "dsa/sarray.h"


static mc_cmp comparator_i16(const void* data_1, const void* data_2) 
{
  const int16_t a = *(int16_t*)data_1;
  const int16_t b = *(int16_t*)data_2;
  return (a > b) - (a < b);
}

static mc_cmp comparator_str(const void* a, const void* b) 
{
  return strcmp(a, b);
}

static void fill_i16(mc_sarray array)
{
  const uint8_t capacity = mc_sarray_get_capacity(array).value;

  for (uint16_t index = 0; index < capacity; index++) {
    int16_t x = (index * 100) + 600;
    mc_sarray_insert(array, &x);
  }
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
  char memory[20];
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
  
  result = mc_sarray_init(mc_span(memory, sizeof(memory)), sizeof(int16_t), 10, comparator_i16);
  if ((MC_ERR_OUT_OF_RANGE != result.result) || (NULL != result.data)) {
    return MC_ERR_OUT_OF_RANGE;
  }

  return MC_SUCCESS;
}

static int test_correct_creation_i16()
{
  int16_t memory[25];
  
  mc_result_ptr result_ptr = mc_sarray_init(
    mc_span(memory, sizeof(memory)),
    sizeof(int16_t), 
    10, 
    comparator_i16);
  mc_sarray array = result_ptr.data;
  if ((MC_SUCCESS != result_ptr.result) || (NULL == array)) {
    return MC_ERR_BAD_ALLOC;
  }

  mc_result_u32 result_u32 = mc_sarray_get_capacity(array);
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

static int test_correct_creation_str()
{
  char memory[60];
  mc_result_ptr result_ptr = {0};
  mc_result_u32 result_u32 = {0};
  
  result_ptr = mc_sarray_init(
    mc_span(memory, sizeof(memory)), 
    10, 
    1, 
    comparator_str);
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
  if ((MC_SUCCESS != result_u32.result) || (10 != result_u32.value)) {
    return MC_ERR_BAD_ALLOC;
  }
  
  return MC_SUCCESS;
}

static int test_empty()
{
  int16_t memory[25];
  mc_result_bool result_bool = {0};
  mc_result_ptr result_ptr = {0};
  
  mc_sarray array = mc_sarray_init(
    mc_span(memory, sizeof(memory)), 
    sizeof(int16_t), 
    10, 
    comparator_i16).data;
  
  result_bool = mc_sarray_is_empty(array);
  if ((MC_SUCCESS != result_bool.result) || (false == result_bool.value)) {
    return MC_ERR_RUNTIME;
  }
  
  mc_result_u32 result_u32 = mc_sarray_get_count(array);
  if ((MC_SUCCESS != result_u32.result) || (0 != result_u32.value)) {
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
  
  result_u32 = mc_sarray_get_count(array);
  if ((MC_SUCCESS != result_u32.result) || (1 != result_u32.value)) {
    return MC_ERR_RUNTIME;
  }
  
  return MC_SUCCESS;
}

static int test_insert_on_empty()
{
  int16_t memory[25];
  
  mc_sarray array = mc_sarray_init(
    mc_span(memory, sizeof(memory)), 
    sizeof(int16_t), 
    10, 
    comparator_i16).data;
  int16_t value = 42;
  mc_result result = mc_sarray_insert(array, &value);
  if (MC_SUCCESS != result) {
    return result;
  }
  
  if (mc_sarray_is_empty(array).value || (1 != mc_sarray_get_count(array).value)) {
    return MC_ERR_RUNTIME;
  }
    
  mc_result_ptr result_ptr = mc_sarray_get(array, 0);
  if ((MC_SUCCESS != result_ptr.result) || (NULL == result_ptr.data)) {
    return MC_ERR_OUT_OF_RANGE;
  }
  if (value != *(int16_t*)result_ptr.data) {
    return MC_ERR_RUNTIME;
  }
  
  return MC_SUCCESS;
}

static int test_insert_ascending()
{
  uint16_t memory[25];
  mc_sarray array = mc_sarray_init(
    mc_span(memory, sizeof(memory)), 
    sizeof(int16_t), 
    10, 
    comparator_i16).data;

  const uint8_t capacity = mc_sarray_get_capacity(array).value;

  for (uint16_t index = 0; index < capacity; index++) {
    int16_t x = (index * 100) + 600;
    
    mc_result result = mc_sarray_insert(array, &x);
    if (MC_SUCCESS != result) {
      return result;
    }

    if ((index + 1) != mc_sarray_get_count(array).value) {
      return MC_ERR_BAD_ALLOC;
    }

    mc_result_ptr result_ptr = mc_sarray_find(array, &x);
    if ((MC_SUCCESS != result_ptr.result) || (NULL == result_ptr.data)) {
      return result_ptr.result;
    }
    if (x != *(int16_t*)result_ptr.data) {
      return MC_ERR_OUT_OF_RANGE;
    }
  }
    
  return MC_SUCCESS;
}

static int test_insert_descending()
{
  uint16_t memory[25];
  mc_sarray array = mc_sarray_init(
    mc_span(memory, sizeof(memory)), 
    sizeof(int16_t), 
    10, 
    comparator_i16).data;

  const uint8_t capacity = mc_sarray_get_capacity(array).value;

  for (uint16_t index = 0; index < capacity; index++) {
    int16_t x = -index * 10;
    mc_result result = mc_sarray_insert(array, &x);
    if (MC_SUCCESS != result) {
      return result;
    }

    if ((index + 1) != mc_sarray_get_count(array).value) {
      return MC_ERR_BAD_ALLOC;
    }

    mc_result_ptr result_ptr = mc_sarray_find(array, &x);
    if ((MC_SUCCESS != result_ptr.result) || (NULL == result_ptr.data)) {
      return result_ptr.result;
    }
    if (x != *(int16_t*)result_ptr.data) {
      return MC_ERR_OUT_OF_RANGE;
    }
  }
    
  return MC_SUCCESS;
}

static int test_insert_not_ordered()
{
  uint16_t memory[25];
  mc_sarray array = mc_sarray_init(
    mc_span(memory, sizeof(memory)), 
    sizeof(int16_t), 
    10, 
    comparator_i16).data;

  const uint8_t capacity = mc_sarray_get_capacity(array).value;
  int16_t nums[] = {7, 19, 0, 9, -5, 67, 3, 4, -8, -8};

  for (uint16_t index = 0; index < capacity; index++) {
    mc_result result = mc_sarray_insert(array, &nums[index]);
    if (MC_SUCCESS != result) {
      return result;
    }

    if ((index + 1) != mc_sarray_get_count(array).value) {
      return MC_ERR_BAD_ALLOC;
    }

    mc_result_ptr result_ptr = mc_sarray_find(array, &nums[index]);
    if ((MC_SUCCESS != result_ptr.result) || (NULL == result_ptr.data)) {
      return result_ptr.result;
    }
    if (nums[index] != *(int16_t*)result_ptr.data) {
      return MC_ERR_OUT_OF_RANGE;
    }
  }
    
  return MC_SUCCESS;
}

static int test_insert_on_full()
{
  int16_t memory[25];
  
  mc_sarray array = mc_sarray_init(
    mc_span(memory, sizeof(memory)), 
    sizeof(int16_t), 
    10, 
    comparator_i16).data;
  fill_i16(array);
  mc_result result = mc_sarray_insert(array, &(int16_t){42});// TODO(MN): Define a micro
  if (MC_ERR_OUT_OF_RANGE != result) {
    return result;
  }
  
  if (mc_sarray_is_empty(array).value || (10 != mc_sarray_get_count(array).value)) {
    return MC_ERR_RUNTIME;
  }
  
  return MC_SUCCESS;
}

static int test_get()
{
  mc_result_u32 size_res = mc_sarray_required_size(sizeof(int16_t), 10);
  uint16_t memory[25];

  mc_sarray array = mc_sarray_init(
    mc_span(memory, sizeof(memory)), 
    sizeof(int16_t), 
    10, 
    comparator_i16).data;
  fill_i16(array);  
  const uint8_t capacity = mc_sarray_get_capacity(array).value;

  for (uint8_t index = 0; index < capacity; index++) {
    const mc_result_ptr result = mc_sarray_get(array, index);
    if ((MC_SUCCESS != result.result) || (NULL == result.data)){
      return result.result;
    }
    
    const int16_t x = (index * 100) + 600;
    if (x != *(int16_t*)result.data) {
      return MC_ERR_OUT_OF_RANGE;
    }
  }
    
  return MC_SUCCESS;
}

static int test_find()
{
  mc_result_u32 size_res = mc_sarray_required_size(sizeof(int16_t), 10);
  uint16_t memory[25];

  mc_sarray array = mc_sarray_init(
    mc_span(memory, sizeof(memory)), 
    sizeof(int16_t), 
    10, 
    comparator_i16).data;
  fill_i16(array); 
  const uint8_t capacity = mc_sarray_get_capacity(array).value;

  uint8_t index = capacity;
  while (index--) {
    const mc_result result = mc_sarray_remove(array, index);
    if (MC_SUCCESS != result){
      return result;
    }

    if (index != mc_sarray_get_count(array).value) {
      return MC_ERR_BAD_ALLOC;
    }
    
    int16_t x = (index * 100) + 600;
    const mc_result_ptr result_ptr = mc_sarray_find(array, &x);
    if ((MC_SUCCESS != result_ptr.result) || (NULL != result_ptr.data)) {
      return MC_ERR_OUT_OF_RANGE;
    }
  }
    
  return MC_SUCCESS;
}

static int test_clear_when_empty()
{
  int16_t memory[25];
  mc_sarray array = mc_sarray_init(
    mc_span(memory, sizeof(memory)), 
    sizeof(int16_t), 
    10, 
    comparator_i16).data;
  
  mc_result result = mc_sarray_clear(array);
  if (MC_SUCCESS != result) {
    return result;
  }

  if (!mc_sarray_is_empty(array).value || (0 != mc_sarray_get_count(array).value)) {
    return MC_ERR_RUNTIME;
  }

  return MC_SUCCESS;
}

static int test_clear()
{
  int16_t memory[25];
  mc_sarray array = mc_sarray_init(
    mc_span(memory, sizeof(memory)), 
    sizeof(int16_t), 
    10, 
    comparator_i16).data;
  
  mc_sarray_insert(array, &(int16_t){54});
  mc_sarray_insert(array, &(int16_t){514});
  mc_sarray_insert(array, &(int16_t){7});

  mc_result result = mc_sarray_clear(array);
  if (MC_SUCCESS != result) {
    return result;
  }

  if (!mc_sarray_is_empty(array).value || (0 != mc_sarray_get_count(array).value)) {
    return MC_ERR_RUNTIME;
  }

  return MC_SUCCESS;
}

static int test_clear_when_full()
{
  int16_t memory[25];
  mc_sarray array = mc_sarray_init(
    mc_span(memory, sizeof(memory)), 
    sizeof(int16_t), 
    10, 
    comparator_i16).data;
  
  fill_i16(array);

  mc_result result = mc_sarray_clear(array);
  if (MC_SUCCESS != result) {
    return result;
  }

  if (!mc_sarray_is_empty(array).value || (0 != mc_sarray_get_count(array).value)) {
    return MC_ERR_RUNTIME;
  }

  return MC_SUCCESS;
}

static int test_remove_when_empty()
{
  int16_t memory[25];
  
  mc_sarray array = mc_sarray_init(
    mc_span(memory, sizeof(memory)), 
    sizeof(int16_t), 
    10, 
    comparator_i16).data;
  
  mc_result result = mc_sarray_remove(array, 0);
  if (MC_ERR_OUT_OF_RANGE != result) {
    return result;
  }

  result = mc_sarray_remove(array, 17);
  if (MC_ERR_OUT_OF_RANGE != result) {
    return result;
  }

  result = mc_sarray_remove(array, -1);
  if (MC_ERR_OUT_OF_RANGE != result) {
    return result;
  }
  
  if (!mc_sarray_is_empty(array).value || (0 != mc_sarray_get_count(array).value)) {
    return MC_ERR_RUNTIME;
  }
  
  return MC_SUCCESS;
}

static int test_remove_descending()
{
  int16_t memory[25];
  
  mc_sarray array = mc_sarray_init(
    mc_span(memory, sizeof(memory)), 
    sizeof(int16_t), 
    10, 
    comparator_i16).data;
  fill_i16(array);

  uint8_t index = mc_sarray_get_capacity(array).value;
  while (index--) {
    mc_result result = mc_sarray_remove(array, index);
    if (MC_ERR_OUT_OF_RANGE != result) {
      return result;
    }
  }
  
  return MC_SUCCESS;
}

static int test_remove_ascending()
{
  int16_t memory[25];
  
  mc_sarray array = mc_sarray_init(
    mc_span(memory, sizeof(memory)), 
    sizeof(int16_t), 
    10, 
    comparator_i16).data;
  fill_i16(array);

  uint8_t index = mc_sarray_get_capacity(array).value;
  while (index--) {
    mc_result result = mc_sarray_remove(array, 0);
    if (MC_ERR_OUT_OF_RANGE != result) {
      return result;
    }
  }
  
  return MC_SUCCESS;
}

static int test_remove_middle()
{
  int16_t memory[25];
  
  mc_sarray array = mc_sarray_init(
    mc_span(memory, sizeof(memory)), 
    sizeof(int16_t), 
    10, 
    comparator_i16).data;
  fill_i16(array);

  uint8_t index = mc_sarray_get_capacity(array).value;
  while (index--) {
    const uint32_t mid = (mc_sarray_get_count(array).value - 1) / 2;
    mc_result result = mc_sarray_remove(array, mid);
    if (MC_ERR_OUT_OF_RANGE != result) {
      return result;
    }
  }
  
  return MC_SUCCESS;
}

int main()
{
  printf("[MICRO CORE - DSA - SORTED_ARRAY - VERSION]: %u.%u.%u\n", MC_VERSION_MAJOR, MC_VERSION_MINOR, MC_VERSION_PATCH);
  uint32_t test_count = 0;
  uint32_t test_failed_count = 0;

  test_required_size();

  printf("[test_required_size]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_required_size();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_invalid_creation]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_invalid_creation();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_correct_creation_i16]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_correct_creation_i16();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_correct_creation_str]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_correct_creation_str();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_empty]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_empty();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_insert_on_empty]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_insert_on_empty();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_insert_ascending]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_insert_ascending();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_insert_descending]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_insert_descending();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_insert_not_ordered]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_insert_not_ordered();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_insert_on_full]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_insert_on_full();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_get]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_get();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_find]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_find();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_clear_when_empty]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_clear_when_empty();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_clear]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_clear();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_clear_when_full]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_clear_when_full();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_remove_when_empty]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_remove_when_empty();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_remove_descending]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_remove_descending();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_remove_ascending]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_remove_ascending();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[test_remove_middle]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_remove_middle();
    test_failed_count += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  if (0 != test_count) {
    printf("%u Tests ran, %u tests failed\n", test_count, test_failed_count);
    if (0 != test_failed_count) {
      printf("#%d Tests failed\n", test_failed_count);
      return MC_ERR_RUNTIME;
    }

    printf("passed\n");
  }
  return MC_SUCCESS;
}
