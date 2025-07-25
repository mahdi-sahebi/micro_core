/* TODO(MN): Remove asc, dsc, random. also for insert.
 */

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

static int8_t comparator_str(const void* a, const void* b) 
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

static int test_empty()
{
  int16_t memory[10];
  mc_result_bool result_bool = {0};
  mc_result_ptr result_ptr = {0};
  
  mc_sarray array = mc_sarray_init(mc_span(memory, sizeof(memory)), sizeof(int16_t), 10, comparator_i16).data;
  
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
  int16_t memory[10];
  mc_span buffer = mc_span(memory, sizeof(memory));
  
  mc_sarray array = mc_sarray_init(buffer, sizeof(int16_t), 10, comparator_i16).data;
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

static int test_insert()
{
  uint16_t memory[10];
  mc_span buffer = mc_span(memory, sizeof(memory));
  mc_sarray array = mc_sarray_init(buffer, sizeof(int16_t), 10, comparator_i16).data;

  const uint8_t capacity = mc_sarray_get_capacity(array).value;

  for (uint16_t index = 0; index < capacity; index++) {
    mc_result result = mc_sarray_insert(array, &(int16_t){(index * 100) + 600});

    if (MC_SUCCESS == result) {
      return result;
    }
  }
    
  return MC_SUCCESS;
}

static int test_insert_on_full()
{
  int16_t memory[10];
  mc_span buffer = mc_span(memory, sizeof(memory));
  
  mc_sarray array = mc_sarray_init(buffer, sizeof(int16_t), 10, comparator_i16).data;
  fill_i16(array);
  mc_result result = mc_sarray_insert(array, &(int16_t){42});// TODO(MN): Define a micro
  if (MC_ERR_OUT_OF_RANGE != result) {
    return result;
  }
  
  if (mc_sarray_is_empty(array).value || (1 != mc_sarray_get_count(array).value)) {
    return MC_ERR_RUNTIME;
  }
  
  return MC_SUCCESS;
}

static int test_multiple_data()
{
  mc_result_u32 size_res = mc_sarray_required_size(sizeof(int16_t), 10);
  uint16_t memory[10];
  mc_span buffer = mc_span(memory, sizeof(memory));
  
  mc_sarray array = mc_sarray_init(buffer, sizeof(int16_t), 10, comparator_i16).data;
  const uint8_t capacity = mc_sarray_get_capacity(array).value;

  fill_i16(array);


  /* Insert when is full */
  int16_t x = 100;
  mc_result result = mc_sarray_insert(array, &x);
  if (MC_SUCCESS == result) {
    return MC_ERR_BAD_ALLOC;
  }
  

  /* Verify content */
  for (uint8_t index = 0; index < capacity; index++) {
    const int16_t x = (index * 100) + 600;
    const mc_result_ptr result = mc_sarray_get(array, index);
    if ((MC_SUCCESS != result.result) || (NULL == result.data)){
      return result.result;
    }
    
    if (x != *(int16_t*)result.data) {
      return MC_ERR_OUT_OF_RANGE;
    }
  }


  /* Empty */
  uint8_t index = capacity;
  while (index--) {
    const mc_result result = mc_sarray_remove(array, index);
    if (MC_SUCCESS != result){
      return result;
    }

    if ((capacity - index + 1) != mc_sarray_get_count(array).value) {
      return MC_ERR_BAD_ALLOC;
    }
    
    int16_t x = (index * 100) + 600;
    const mc_result_ptr result_ptr = mc_sarray_find(array, &x);
    if ((MC_ERR_OUT_OF_RANGE != result_ptr.result) || (NULL != result_ptr.data)) {
      return MC_ERR_BAD_ALLOC;
    }
  }


  if (0 != mc_sarray_get_count(array).value) {
    return MC_ERR_BAD_ALLOC;
  }
  result = mc_sarray_remove(array, index);
  if (MC_SUCCESS != result){
    return result;
  }
    
  return MC_SUCCESS;
}

static int test_clear_when_empty()
{
  int16_t memory[10];
  mc_sarray array = mc_sarray_init(mc_span(memory, sizeof(memory)), sizeof(int16_t), 10, comparator_i16).data;
  
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
  int16_t memory[10];
  mc_sarray array = mc_sarray_init(mc_span(memory, sizeof(memory)), sizeof(int16_t), 10, comparator_i16).data;
  
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
  int16_t memory[10];
  mc_sarray array = mc_sarray_init(mc_span(memory, sizeof(memory)), sizeof(int16_t), 10, comparator_i16).data;
  
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
  int16_t memory[10];
  mc_span buffer = mc_span(memory, sizeof(memory));
  
  mc_sarray array = mc_sarray_init(buffer, sizeof(int16_t), 10, comparator_i16).data;
  
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
  
  if (mc_sarray_is_empty(array).value || (0 != mc_sarray_get_count(array).value)) {
    return MC_ERR_RUNTIME;
  }
  
  return MC_SUCCESS;
}

static int test_remove()
{
  int16_t memory[10];
  mc_span buffer = mc_span(memory, sizeof(memory));
  
  mc_sarray array = mc_sarray_init(buffer, sizeof(int16_t), 10, comparator_i16).data;
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

  printf("[test_insert]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_insert();
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

  printf("[test_multiple_data]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_multiple_data();
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

  printf("[test_remove]\n");
  {
    test_count++;
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = test_remove();
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
