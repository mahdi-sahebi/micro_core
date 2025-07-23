#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "core/error.h"
#include "core/version.h"
#include "core/time.h"
#include "alg/algorithm.h"


static int8_t comparator(const void* data_1, const void* data_2) 
{
  const int16_t a = *(const int16_t*)data_1;
  const int16_t b = *(const int16_t*)data_2;
  return (a > b) - (a < b);
}

static int search_invalid_arguments()
{
  const mc_span buffer = mc_span(NULL, 0);
  int16_t key = 5;
  mc_result_ptr result = {0};

  result = mc_alg_lower_bound(buffer, &key, sizeof(key), comparator);
  if (NULL != result.data) {
    return MC_ERR_RUNTIME;
  }
  if (MC_ERR_INVALID_ARGUMENT != result.result) {
    return MC_ERR_RUNTIME;
  }


  result = mc_alg_lower_bound(buffer, &key, sizeof(key), NULL);
  if (NULL != result.data) {
    return MC_ERR_RUNTIME;
  }
  if (MC_ERR_INVALID_ARGUMENT != result.result) {
    return MC_ERR_RUNTIME;
  }


  result = mc_alg_lower_bound(buffer, &key, 0, comparator);
  if (NULL != result.data) {
    return MC_ERR_RUNTIME;
  }
  if (MC_ERR_INVALID_ARGUMENT != result.result) {
    return MC_ERR_RUNTIME;
  }


  result = mc_alg_lower_bound(buffer, NULL, sizeof(key), comparator);
  if (NULL != result.data) {
    return MC_ERR_RUNTIME;
  }
  if (MC_ERR_INVALID_ARGUMENT != result.result) {
    return MC_ERR_RUNTIME;
  }

  
  return MC_SUCCESS;
}

static int search_empty_buffer()
{
  int16_t array[] = {};
  const mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 5;

  const mc_result_ptr result = mc_alg_lower_bound(buffer, &key, sizeof(key), comparator);
  if ((MC_SUCCESS != result.result) || (NULL != result.data)) {
    return result.result;
  }
  return MC_SUCCESS;
}

static int search_present()
{
  int16_t array[] = {1, 3, 5, 7, 9};
  const mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 5;

  const mc_result_ptr result = mc_alg_lower_bound(buffer, &key, sizeof(key), comparator);
  if ((MC_SUCCESS != result.result) || (result.data != &array[2])) {
    return result.result;
  }
  return MC_SUCCESS;
}

static int search_not_present()
{
  int16_t array[] = {1, 3, 5, 7, 9};
  const mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 6;

  const mc_result_ptr result = mc_alg_lower_bound(buffer, &key, sizeof(key), comparator);
  if ((MC_SUCCESS != result.result) || (result.data != &array[3])) {
    return result.result;
  }
  return MC_SUCCESS;
}

static int search_greater_than_all()
{
  int16_t array[] = {1, 3, 5, 7, 9};
  const mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 10;

  const mc_result_ptr result = mc_alg_lower_bound(buffer, &key, sizeof(key), comparator);
  if ((MC_SUCCESS != result.result) || (result.data != &array[5])) {
    return result.result;
  }
  return MC_SUCCESS;
}

static int search_less_than_all()
{
  int16_t array[] = {1, 3, 5, 7, 9};
  const mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 0;
  
  const mc_result_ptr result = mc_alg_lower_bound(buffer, &key, sizeof(key), comparator);
  if ((MC_SUCCESS != result.result) || (result.data != &array[0])) {
    return result.result;
  }
  return MC_SUCCESS;
}

static int search_first_duplicate()
{
  int16_t array[] = {1, 2, 2, 2, 3};
  mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 2;
  
  const mc_result_ptr result = mc_alg_lower_bound(buffer, &key, sizeof(key), comparator);
  if ((MC_SUCCESS != result.result) || (result.data != &array[1])) {
    return result.result;
  }
  return MC_SUCCESS;
}

static int search_element_end()
{
  int16_t array[] = {1, 2, 2, 2, 3};
  mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 3;

  const mc_result_ptr result = mc_alg_lower_bound(buffer, &key, sizeof(key), comparator);
  if ((MC_SUCCESS != result.result) || (result.data != &array[4])) {
    return result.result;
  }
  return MC_SUCCESS;
}

static int search_not_prepresent_with_duplicate()
{
  int16_t array[] = {2, 2, 2, 2, 2};
  mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 2;

  const mc_result_ptr result = mc_alg_lower_bound(buffer, &key, sizeof(key), comparator);
  if ((MC_SUCCESS != result.result) || (result.data != &array[0])) {
    return result.result;
  }
  return MC_SUCCESS;
}

static int search_large_array()
{
  int16_t array[10000] = {0};
  for (uint32_t index = 0; index < 10000; index++) {
    array[index] = index;
  }
  const mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 9999;

  const mc_result_ptr result = mc_alg_lower_bound(buffer, &key, sizeof(key), comparator);
  if ((MC_SUCCESS != result.result) || (result.data != &array[9999])) {
    return result.result;
  }
  return MC_SUCCESS;
}

int main()
{
  printf("[MICRO CORE - ALG - VERSION]: %u.%u.%u\n", MC_VERSION_MAJOR, MC_VERSION_MINOR, MC_VERSION_PATCH);
  uint32_t total_failed = 0;

  printf("[search_invalid_arguments]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_invalid_arguments();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_empty_buffer]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_empty_buffer();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_present]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_present();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_not_present]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_not_present();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_greater_than_all]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_greater_than_all();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_less_than_all]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_less_than_all();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_first_duplicate]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_first_duplicate();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_element_end]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_element_end();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_not_prepresent_with_duplicate]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_not_prepresent_with_duplicate();
    total_failed += (MC_SUCCESS != result);
    if (MC_SUCCESS != result) {
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_large_array]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_large_array();
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
