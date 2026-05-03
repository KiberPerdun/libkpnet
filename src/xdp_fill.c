//
// Created by KiberPerdun on 03.05.2026.
//

#include "xdp.h"

u32
xdp_fill (xdp_t *xsk, umem_stack_t *stack, u32 max_n)
{
  u32 prod, cons, n, free_entries, i, idx;
  u64 addr;

  prod = *xsk->fill_ring_producer;
  cons = *xsk->fill_ring_consumer;

  free_entries = (RING_MASK + 1) - (prod - cons);

  n = free_entries < max_n ? free_entries : max_n;
  if (__builtin_expect (0 == n, 0))
    return 0;

  for (i = 0; i < n; ++i)
    {
      addr = umem_stack_pop (stack);

      if (__builtin_expect (addr == (u64)-1, 0))
        {
          n = i;
          break;
        }

      idx = (prod + i) & RING_MASK;
      xsk->fill_ring[idx] = addr;
    }

  if (n > 0)
    {
      ring_smp_wmb ();
      *xsk->fill_ring_producer = prod + n;
    }

  return n;
}