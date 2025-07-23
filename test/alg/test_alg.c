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

static int search_null_buffer()
{
  const mc_span buffer = mc_span(NULL, 0);
  int16_t key = 5;
  
  const int16_t* itr = mc_alg_lower_bound(buffer, sizeof(key), &key, comparator);

  if (NULL != itr) {
    return MC_ERR_RUNTIME;
  }
  return MC_SUCCESS;
}

static int search_null_comparator()
{
  int16_t array[] = {1, 3, 5, 7, 9};
  const mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 5;
  
  const int16_t* itr = mc_alg_lower_bound(buffer, sizeof(key), &key, NULL);
  
  if (NULL != itr) {
    return MC_ERR_RUNTIME;
  }
  return MC_SUCCESS;
}

static int search_empty_buffer()
{
  int16_t array[] = {};
  const mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 5;

  const int16_t* itr = mc_alg_lower_bound(buffer, sizeof(key), &key, comparator);

  if (itr != array) {
    return MC_ERR_RUNTIME;
  }
  return MC_SUCCESS;
}

static int search_present()
{
  int16_t array[] = {1, 3, 5, 7, 9};
  const mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 5;

  const int16_t* itr = mc_alg_lower_bound(buffer, sizeof(key), &key, comparator);
  
  if (itr != &array[2]) {
    return MC_ERR_RUNTIME;
  }
  return MC_SUCCESS;
}

static int search_not_present()
{
  int16_t array[] = {1, 3, 5, 7, 9};
  const mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 6;

  const int16_t* itr = mc_alg_lower_bound(buffer, sizeof(key), &key, comparator);
  
  if (itr != &array[3]) {
    return MC_ERR_RUNTIME;
  }
  return MC_SUCCESS;
}

static int search_greater_than_all()
{
  int16_t array[] = {1, 3, 5, 7, 9};
  const mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 10;

  const int16_t* itr = mc_alg_lower_bound(buffer, sizeof(key), &key, comparator);
  
  if (itr != &array[5]) {
    return MC_ERR_RUNTIME;
  }
  return MC_SUCCESS;
}

static int search_less_than_all()
{
  int16_t array[] = {1, 3, 5, 7, 9};
  const mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 0;
  
  const int16_t* itr = mc_alg_lower_bound(buffer, sizeof(key), &key, comparator);
  
  if (itr != &array[0]) {
    return MC_ERR_RUNTIME;
  }
  return MC_SUCCESS;
}

static int search_first_duplicate()
{
  int16_t array[] = {1, 2, 2, 2, 3};
  mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 2;
  const int16_t* itr = mc_alg_lower_bound(buffer, sizeof(key), &key, comparator);
  
  if (itr != &array[1]) {
    return MC_ERR_RUNTIME;
  }
  return MC_SUCCESS;
}

static int search_element_end()
{
  int16_t array[] = {1, 2, 2, 2, 3};
  mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 3;

  const int16_t* itr = mc_alg_lower_bound(buffer, sizeof(key), &key, comparator);
  
  if (itr != &array[4]) {
    return MC_ERR_RUNTIME;
  }
  return MC_SUCCESS;
}

static int search_not_prepresent_with_duplicate()
{
  int16_t array[] = {2, 2, 2, 2, 2};
  mc_span buffer = mc_span(array, sizeof(array));
  int16_t key = 2;

  const int16_t* itr = mc_alg_lower_bound(buffer, sizeof(key), &key, comparator);
  
  if (itr != &array[0]) {
    return MC_ERR_RUNTIME;
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

  const int16_t* itr = mc_alg_lower_bound(buffer, sizeof(key), &key, comparator);
  
  if (itr != &array[9999]) {
    return MC_ERR_RUNTIME;
  }
  return MC_SUCCESS;
}

int main()
{
  printf("[MICRO CORE - ALG - VERSION]: %u.%u.%u\n", MC_VERSION_MAJOR, MC_VERSION_MINOR, MC_VERSION_PATCH);
  mc_result total_result = MC_SUCCESS;

  printf("[search_null_buffer]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_null_buffer();
    if (MC_SUCCESS != result) {
      if (MC_SUCCESS == total_result) {
        total_result = result;
      }
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_null_comparator]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_null_comparator();
    if (MC_SUCCESS != result) {
      if (MC_SUCCESS == total_result) {
        total_result = result;
      }
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_empty_buffer]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_empty_buffer();
    if (MC_SUCCESS != result) {
      if (MC_SUCCESS == total_result) {
        total_result = result;
      }
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_present]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_present();
    if (MC_SUCCESS != result) {
      if (MC_SUCCESS == total_result) {
        total_result = result;
      }
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_not_present]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_not_present();
    if (MC_SUCCESS != result) {
      if (MC_SUCCESS == total_result) {
        total_result = result;
      }
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_greater_than_all]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_greater_than_all();
    if (MC_SUCCESS != result) {
      if (MC_SUCCESS == total_result) {
        total_result = result;
      }
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_less_than_all]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_less_than_all();
    if (MC_SUCCESS != result) {
      if (MC_SUCCESS == total_result) {
        total_result = result;
      }
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_first_duplicate]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_first_duplicate();
    if (MC_SUCCESS != result) {
      if (MC_SUCCESS == total_result) {
        total_result = result;
      }
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_element_end]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_element_end();
    if (MC_SUCCESS != result) {
      if (MC_SUCCESS == total_result) {
        total_result = result;
      }
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_not_prepresent_with_duplicate]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_not_prepresent_with_duplicate();
    if (MC_SUCCESS != result) {
      if (MC_SUCCESS == total_result) {
        total_result = result;
      }
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("[search_large_array]\n");
  {
    const mc_time_t bgn_time_us = mc_now_u();
    const mc_result result = search_large_array();
    if (MC_SUCCESS != result) {
      if (MC_SUCCESS == total_result) {
        total_result = result;
      }
      printf("FAILED: %u\n\n", result);
    } else {
      printf("PASSED - %u(us)\n\n", (uint32_t)(mc_now_u() - bgn_time_us));
    }
  }

  printf("passed\n");
  return total_result;
}
