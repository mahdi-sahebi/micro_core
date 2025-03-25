// TODO(MN): make full.
// TODO(MN): check all bytes pattern after making full. for capacity 1,2,4,8 bytes 

#include <stdio.h>
#include "dsa/memory/memlin.h"


enum sizes
{
  SIZE_08_BIT = 0x42,
  SIZE_16_BIT = 0x1a8,
  SIZE_32_BIT = 0xe3744f,
};

/* We won't write 32-bit size on this buffer. Only creation */
static char Buffer[0x3e8];


static int create_8()
{
  const mc_span buffer = mc_span(Buffer, SIZE_08_BIT);

  const mc_error_ptr result = mc_memlin_create(buffer);
  if ((MC_SUCCESS != result.result) || (NULL == result.data)) {
    return result.result;
  }

  return MC_SUCCESS;
}

static int create_16()
{
  const mc_span buffer = mc_span(Buffer, SIZE_16_BIT);

  const mc_error_ptr result = mc_memlin_create(buffer);
  if ((MC_SUCCESS != result.result) || (NULL == result.data)) {
    return result.result;
  }

  return MC_SUCCESS;
}

static int create_32()
{
  const mc_span buffer = mc_span(Buffer, SIZE_32_BIT);

  const mc_error_ptr result = mc_memlin_create(buffer);
  if ((MC_SUCCESS != result.result) || (NULL == result.data)) {
    return result.result;
  }

  return MC_SUCCESS;
}

static int create_invalid()
{
  const mc_span buffer = mc_span(NULL, SIZE_08_BIT);

  const mc_error_ptr result = mc_memlin_create(buffer);
  if ((MC_ERR_BAD_ALLOC != result.result) || (NULL != result.data)) {
    return result.result;
  }

  return MC_SUCCESS;
}

static int get_capacity_8()
{
  const mc_span buffer = mc_span(Buffer, SIZE_08_BIT);
  // TODO(MN): get paylod_size/get reserved size. to comapreable
  const mc_memlin* const memory = mc_memlin_create(buffer).data;
  if (buffer.size >= mc_memlin_get_capacity(memory).value) {
    return MC_ERR_BAD_ALLOC;
  }
  
  return MC_SUCCESS;
}

static int get_capacity_16()
{
  const mc_span buffer = mc_span(Buffer, SIZE_16_BIT);

  const mc_memlin* const memory = mc_memlin_create(buffer).data;
  if (buffer.size >= mc_memlin_get_capacity(memory).value) {
    return MC_ERR_BAD_ALLOC;
  }
  
  return MC_SUCCESS;
}

static int get_capacity_32()
{
  const mc_span buffer = mc_span(Buffer, SIZE_32_BIT);

  const mc_memlin* const memory = mc_memlin_create(buffer).data;
  if (buffer.size >= mc_memlin_get_capacity(memory).value) {
    return MC_ERR_BAD_ALLOC;
  }
  
  return MC_SUCCESS;
}

static int get_size_8()
{
  const mc_span buffer = mc_span(Buffer, SIZE_08_BIT);
  mc_memlin* const memory = mc_memlin_create(buffer).data;

  const mc_error_u32 result = mc_memlin_get_size(memory);
  if ((MC_SUCCESS != result.result) || (0 != result.value)) {
    return result.result;
  }
  
  return MC_SUCCESS;
}

static int get_size_16()
{
  const mc_span buffer = mc_span(Buffer, SIZE_16_BIT);
  mc_memlin* const memory = mc_memlin_create(buffer).data;

  const mc_error_u32 result = mc_memlin_get_size(memory);
  if ((MC_SUCCESS != result.result) || (0 != result.value)) {
    return result.result;
  }
  
  return MC_SUCCESS;
}

static int get_size_32()
{
  const mc_span buffer = mc_span(Buffer, SIZE_32_BIT);
  mc_memlin* const memory = mc_memlin_create(buffer).data;

  const mc_error_u32 result = mc_memlin_get_size(memory);
  if ((MC_SUCCESS != result.result) || (0 != result.value)) {
    return result.result;
  }
  
  return MC_SUCCESS;
}

static int destroy()
{
  const mc_span buffer = mc_span(Buffer, SIZE_08_BIT);
  mc_error_ptr result;

  result = mc_memlin_create(buffer);
  if ((MC_SUCCESS != result.result) || (NULL == result.data)) {
    return result.result;
  }

  mc_memlin* memory = result.data;
  result.result = mc_memlin_destroy(&memory);
  if ((MC_SUCCESS != result.result) || (NULL != memory)) {
    return result.result;
  }

  return MC_SUCCESS;
}

int main()
{
  mc_error result = MC_SUCCESS;

  result = create_8();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: create_8 - %u\n", result);
    return result;
  }

  result = create_16();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: create_16 - %u\n", result);
    return result;
  }

  result = create_32();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: create_32 - %u\n", result);
    return result;
  }

  result = create_invalid();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: create_invalid - %u\n", result);
    return result;
  }

  result = get_capacity_8();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: get_capacity_8 - %u\n", result);
    return result;
  }

  result = get_capacity_16();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: get_capacity_16 - %u\n", result);
    return result;
  }

  result = get_capacity_32();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: get_capacity_32 - %u\n", result);
    return result;
  }

  result = get_size_8();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: get_size_8 - %u\n", result);
    return result;
  }

  result = get_size_16();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: get_size_16 - %u\n", result);
    return result;
  }

  result = get_size_32();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: get_size_32 - %u\n", result);
    return result;
  }


  result = destroy();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: destroy - %u\n", result);
    return result;
  }

  return result;
}

