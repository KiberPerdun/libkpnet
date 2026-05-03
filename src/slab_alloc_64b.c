//
// Created by KiberPerdun on 03.05.2026.
//

#include "hwinfo.h"
#include "xdp.h"

u0 *
slab_alloc_64b (umem_slab_allocator_t *alloc, u8 *umem_base)
{
  u0 *ptr;

  if (__builtin_expect (alloc->free_list != NULL, 1))
    {
      ptr = alloc->free_list;
      alloc->free_list = *(u0 **)ptr;
      return ptr;
    }

  if (__builtin_expect (alloc->cur_pos >= alloc->cur_end, 0))
    {
      u64 offset = umem_stack_pop (alloc->stack);

      if (__builtin_expect (offset == (u64)-1, 0))
        return NULL;

      alloc->cur_pos = umem_base + offset;
      alloc->cur_end = alloc->cur_pos + UMEM_CHUNK_SIZE;
    }

  ptr = alloc->cur_pos;
  alloc->cur_pos += L1_CACHE_BYTES;

  return ptr;
}
