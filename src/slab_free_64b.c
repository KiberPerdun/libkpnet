//
// Created by KiberPerdun on 03.05.2026.
//

#include "xdp.h"

u0
slab_free_64b (umem_slab_allocator_t *alloc, u0 *ptr)
{
  *(u0 **)ptr = alloc->free_list;
  alloc->free_list = ptr;
}