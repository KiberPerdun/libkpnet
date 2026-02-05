//
// Created by KiberPerdun on 10.01.26.
//

#include "xdp.h"

#include <stdlib.h>

umem_stack_t *
create_umem_stack (u32 num_chunks, u32 chunk_size)
{
  umem_stack_t *umem_stack;
  umem_stack = (umem_stack_t *) malloc (sizeof (umem_stack_t));
  if (!umem_stack)
    return NULL;

  umem_stack->addrs = (u64 *) malloc (sizeof (u64) * num_chunks);
  if (!umem_stack->addrs)
    goto cleanup;

  umem_stack->cap = num_chunks;

  for (i64 i = 0; i < num_chunks; ++i)
    umem_stack->addrs[i] = i * chunk_size;

  umem_stack->top = num_chunks;
  return umem_stack;

cleanup:
  free (umem_stack);
  return NULL;
}
