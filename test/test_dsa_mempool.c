// TODO(MN): make full.
// TODO(MN): mc_mempool_create
// TODO(MN): check all bytes pattern after making full. for capacity 1,2,4,8 bytes 

#include <stdio.h>
#include "core/version.h"
#include "dsa/memory/mempool.h"


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
  return MC_ERR_BAD_ALLOC;
}

static int create_16()
{
  return MC_ERR_BAD_ALLOC;
}

static int create_32()
{
  return MC_ERR_BAD_ALLOC;
}

static int create_invalid()
{
  return MC_ERR_BAD_ALLOC;
}

static int get_capacity_8()
{
  return MC_ERR_BAD_ALLOC;
}

static int get_capacity_16()
{
  return MC_ERR_BAD_ALLOC;
}

static int get_capacity_32()
{
  return MC_ERR_BAD_ALLOC;
}

static int get_size_8()
{
  return MC_ERR_BAD_ALLOC;
}

static int get_size_16()
{
  return MC_ERR_BAD_ALLOC;
}

static int get_size_32()
{
  return MC_ERR_BAD_ALLOC;
}

static int destroy()
{
  return MC_ERR_BAD_ALLOC;
}

static int stress()
{
  return MC_ERR_BAD_ALLOC;
}

int main()
{
  printf("[MICRO CORE - DSA - MEMPOOL - VERSION]: %u.%u.%u\n", MC_VERSION_MAJOR, MC_VERSION_MINOR, MC_VERSION_PATCH);

  mc_result result = MC_SUCCESS;

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

  result = stress();
  if (MC_SUCCESS != result) {
    printf("[FAILED]: stress - %u\n", result);
    return result;
  }

  printf("passed\n");
  return result;
}

