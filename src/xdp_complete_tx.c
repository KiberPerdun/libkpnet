//
// Created by KiberPerdun on 10.01.26.
//

#include "xdp.h"

u32
xdp_complete_tx (xdp_t *xsk, u64 *addr, u32 max_n)
{
  u32 prod, cons, n, i, idx;
  xdp_desc_t *desc;

  prod = *xsk->completion_ring_producer;
  cons = *xsk->completion_ring_consumer;
  n = prod - cons;

  if (0 == n)
    return 0;

  if (n > max_n)
    n = max_n;

  ring_smp_rmb ();

  for (i = 0; i < n; ++i)
    {
      idx = (cons + i) & RING_MASK;
      addr[i] = xsk->completion_ring[idx];
    }

  ring_smp_wmb ();
  *xsk->completion_ring_consumer = cons + n;

  return n;
}
