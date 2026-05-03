//
// Created by KiberPerdun on 10.01.26.
//

#include "xdp.h"

u32
xdp_complete_tx (xdp_t *xsk, u64 *out_addrs, u32 max_n)
{
  u32 prod, cons, n, i, idx;

  prod = *xsk->completion_ring_producer;
  cons = *xsk->completion_ring_consumer;

  n = prod - cons;

  if (__builtin_expect (0 == n, 0))
    return 0;

  if (n > max_n)
    n = max_n;

  ring_smp_rmb ();

  for (i = 0; i < n; ++i)
    {
      idx = (cons + i) & RING_MASK;
      out_addrs[i] = xsk->completion_ring[idx];
    }

  ring_smp_wmb ();
  *xsk->completion_ring_consumer = cons + n;

  return n;
}
