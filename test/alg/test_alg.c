#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "core/error.h"
#include "core/version.h"
#include "core/time.h"
#include "alg/algorithm.h"


static float distance_i16(cvoid* data_1, cvoid* data_2) 
{
  const int16_t a = *(const int16_t*)data_1;
  const int16_t b = *(const int16_t*)data_2;

  return a - b;
}

static int lower_bound_invalid_arguments()
{
  int16_t key = 5;
  const mc_buffer buffer = mc_buffer_raw(NULL, 0, sizeof(key));
  mc_u32 result = {0};

  result = mc_alg_lower_bound(buffer, &key, distance_i16);
  if (buffer.capacity != result.value) {
    return MC_ERR_RUNTIME;
  }
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_RUNTIME;
  }

  result = mc_alg_lower_bound(buffer, &key, NULL);
  if (buffer.capacity != result.value) {
    return MC_ERR_RUNTIME;
  }
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_RUNTIME;
  }

  result = mc_alg_lower_bound(mc_buffer_raw(NULL, 0, 0), &key, distance_i16);
  if (buffer.capacity != result.value) {
    return MC_ERR_RUNTIME;
  }
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_RUNTIME;
  }

  result = mc_alg_lower_bound(buffer, NULL, distance_i16);
  if (buffer.capacity != result.value) {
    return MC_ERR_RUNTIME;
  }
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_RUNTIME;
  }

  return MC_SUCCESS;
}

static int lower_bound_empty_buffer()
{
  int16_t array[] = {};
  int16_t key = 5;
  const mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));

  const mc_u32 result = mc_alg_lower_bound(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (buffer.capacity != result.value)) {
    return result.error;
  }
  return MC_SUCCESS;
}

static int lower_bound_present()
{
  int16_t array[] = {1, 3, 5, 7, 9};
  int16_t key = 5;
  const mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));

  const mc_u32 result = mc_alg_lower_bound(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (2 != result.value)) {
    return result.error;
  }
  return MC_SUCCESS;
}

static int lower_bound_not_present()
{
  int16_t array[] = {1, 3, 5, 7, 9};
  int16_t key = 6;
  const mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));

  const mc_u32 result = mc_alg_lower_bound(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (3 != result.value)) {
    return result.error;
  }
  return MC_SUCCESS;
}

static int lower_bound_greater_than_all()
{
  int16_t array[] = {1, 3, 5, 7, 9};
  int16_t key = 10;
  const mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));

  const mc_u32 result = mc_alg_lower_bound(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (5 != result.value)) {
    return result.error;
  }
  return MC_SUCCESS;
}

static int lower_bound_less_than_all()
{
  int16_t array[] = {1, 3, 5, 7, 9};
  int16_t key = 0;
  const mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));
  
  const mc_u32 result = mc_alg_lower_bound(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (0 != result.value)) {
    return result.error;
  }
  return MC_SUCCESS;
}

static int lower_bound_first_duplicate()
{
  int16_t array[] = {1, 2, 2, 2, 3};
  int16_t key = 2;
  mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));
  
  const mc_u32 result = mc_alg_lower_bound(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (1 != result.value)) {
    return result.error;
  }
  return MC_SUCCESS;
}

static int lower_bound_last_element()
{
  int16_t array[] = {1, 2, 2, 2, 3};
  int16_t key = 3;
  mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));

  const mc_u32 result = mc_alg_lower_bound(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (4 != result.value)) {
    return result.error;
  }
  return MC_SUCCESS;
}

static int lower_bound_prepresent_with_duplicate()
{
  int16_t array[] = {2, 2, 2, 2, 2};
  int16_t key = 2;
  mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));

  const mc_u32 result = mc_alg_lower_bound(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (0 != result.value)) {
    return result.error;
  }
  return MC_SUCCESS;
}

static int lower_bound_large_array()
{
  int16_t array[10000] = {0};
  const uint32_t count = sizeof(array) / sizeof(*array);

  for (uint32_t index = 0; index < count; index++) {
    array[index] = index;
  }

  for (uint32_t index = 0; index < count; index++) {
    int16_t key = index;
    const mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));

    const mc_u32 result = mc_alg_lower_bound(buffer, &key, distance_i16);
    if ((MC_SUCCESS != result.error) || (index != result.value)) {
      return result.error;
    }
  }

  return MC_SUCCESS;
}

///////////////////////////////////////////////////////
static int nearest_invalid_arguments()
{
  int16_t key = 5;
  const mc_buffer buffer = mc_buffer_raw(NULL, 0, sizeof(key));
  mc_u32 result = {0};

  result = mc_alg_nearest(buffer, &key, distance_i16);
  if (buffer.capacity != result.value) {
    return MC_ERR_RUNTIME;
  }
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_RUNTIME;
  }

  result = mc_alg_nearest(buffer, &key, NULL);
  if (buffer.capacity != result.value) {
    return MC_ERR_RUNTIME;
  }
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_RUNTIME;
  }

  result = mc_alg_nearest(mc_buffer_raw(NULL, 0, 0), &key, distance_i16);
  if (buffer.capacity != result.value) {
    return MC_ERR_RUNTIME;
  }
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_RUNTIME;
  }

  result = mc_alg_nearest(buffer, NULL, distance_i16);
  if (buffer.capacity != result.value) {
    return MC_ERR_RUNTIME;
  }
  if (MC_ERR_INVALID_ARGUMENT != result.error) {
    return MC_ERR_RUNTIME;
  }

  return MC_SUCCESS;
}

static int nearest_empty_buffer()
{
  int16_t array[] = {};
  int16_t key = 5;
  const mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));

  const mc_u32 result = mc_alg_nearest(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (buffer.capacity != result.value)) {
    return result.error;
  }
  return MC_SUCCESS;
}

static int nearest_present()
{
  int16_t array[] = {1, 3, 5, 7, 9};
  int16_t key = 5;
  const mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));

  const mc_u32 result = mc_alg_nearest(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (2 != result.value)) {
    return result.error;
  }
  return MC_SUCCESS;
}

static int nearest_not_present()
{
  int16_t array[] = {-51, 3, 5, 11, 17};
  int16_t key = 6;
  const mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));

  mc_u32 result = mc_alg_nearest(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (2 != result.value)) {
    return result.error;
  }

  key = 12;
  result = mc_alg_nearest(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (3 != result.value)) {
    return result.error;
  }

  key = 2;
  result = mc_alg_nearest(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (1 != result.value)) {
    return result.error;
  }

  return MC_SUCCESS;
}

static int nearest_greater_than_all()
{
  int16_t array[] = {1, 3, 5, 7, 9};
  int16_t key = 81;
  const mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));

  mc_u32 result = mc_alg_nearest(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (4 != result.value)) {
    return result.error;
  }

  key = 10;
  result = mc_alg_nearest(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (4 != result.value)) {
    return result.error;
  }

  return MC_SUCCESS;
}

static int nearest_less_than_all()
{
  int16_t array[] = {1, 3, 5, 7, 9};
  int16_t key = 0;
  const mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));
  
  mc_u32 result = mc_alg_nearest(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (0 != result.value)) {
    return result.error;
  }
  
  key = -5;
  result = mc_alg_nearest(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (0 != result.value)) {
    return result.error;
  }
  
  return MC_SUCCESS;
}

static int nearest_first_duplicate()
{
  int16_t array[] = {1, 20, 20, 20, 31};
  int16_t key = 17;
  mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));
  
  mc_u32 result = mc_alg_nearest(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (1 != result.value)) {
    return result.error;
  }

  key = 22;
  result = mc_alg_nearest(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (1 != result.value)) {
    return result.error;
  }

  return MC_SUCCESS;
}

static int nearest_last_element()
{
  int16_t array[] = {1, 2, 2, 2, 3};
  int16_t key = 3;
  mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));

  mc_u32 result = mc_alg_nearest(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (4 != result.value)) {
    return result.error;
  }

  key = 4;
  result = mc_alg_nearest(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (4 != result.value)) {
    return result.error;
  }

  return MC_SUCCESS;
}

static int nearest_prepresent_with_duplicate()
{
  int16_t array[] = {2, 2, 2, 2, 2};
  int16_t key = 2;
  mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));

  const mc_u32 result = mc_alg_nearest(buffer, &key, distance_i16);
  if ((MC_SUCCESS != result.error) || (0 != result.value)) {
    return result.error;
  }

  return MC_SUCCESS;
}

static int nearest_large_array()
{
  int16_t array[10000] = {0};
  const uint32_t count = sizeof(array) / sizeof(*array);

  for (uint32_t index = 0; index < count; index++) {
    array[index] = index * 10;
  }

  for (uint32_t index = 0; index < count; index++) {
    int16_t key = index - 1;
    const mc_buffer buffer = mc_buffer_raw(array, sizeof(array), sizeof(key));

    const mc_u32 result = mc_alg_nearest(buffer, &key, distance_i16);
    if ((MC_SUCCESS != result.error) || (index != result.value)) {
      return result.error;
    }
  }

  return MC_SUCCESS;
}

int main()
{
  printf("[MICRO CORE - ALG - VERSION]: %u.%u.%u\n", MC_VERSION_MAJOR, MC_VERSION_MINOR, MC_VERSION_PATCH);
  uint32_t total_failed = 0;

  printf("[lower_bound_invalid_arguments]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = lower_bound_invalid_arguments();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[lower_bound_empty_buffer]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = lower_bound_empty_buffer();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[lower_bound_present]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = lower_bound_present();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[lower_bound_not_present]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = lower_bound_not_present();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[lower_bound_greater_than_all]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = lower_bound_greater_than_all();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[lower_bound_less_than_all]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = lower_bound_less_than_all();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[lower_bound_first_duplicate]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = lower_bound_first_duplicate();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[lower_bound_last_element]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = lower_bound_last_element();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[lower_bound_prepresent_with_duplicate]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = lower_bound_prepresent_with_duplicate();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[lower_bound_large_array]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = lower_bound_large_array();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }
////////////////////////////////////////////////

  printf("[nearest_invalid_arguments]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = nearest_invalid_arguments();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[nearest_empty_buffer]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = nearest_empty_buffer();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[nearest_present]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = nearest_present();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[nearest_not_present]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = nearest_not_present();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[nearest_greater_than_all]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = nearest_greater_than_all();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[nearest_less_than_all]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = nearest_less_than_all();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[nearest_first_duplicate]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = nearest_first_duplicate();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[nearest_last_element]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = nearest_last_element();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[nearest_prepresent_with_duplicate]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = nearest_prepresent_with_duplicate();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[nearest_large_array]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_error result = nearest_large_array();
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
