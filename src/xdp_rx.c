//
// Created by KiberPerdun on 10.01.26.
//

#include "xdp.h"

u32
xdp_rx (xdp_t *xsk, u64 *out, u32 *out_lens, u32 max_n)
{
  u32 prod, cons, n, i, idx;
  xdp_desc_t *desc;

  prod = *xsk->rx_ring_producer;
  cons = *xsk->rx_ring_consumer;
  n = prod - cons;

  if (0 == n)
    return 0;

  if (n > max_n)
    n = max_n;

  ring_smp_rmb ();

  for (i = 0; i < n; ++i)
    {
      idx = (cons + i) & RING_MASK;
      desc = &xsk->rx_ring[idx];
      out[i] = desc->addr;
      out_lens[i] = desc->len;
    }

  ring_smp_wmb ();

  *xsk->rx_ring_consumer = cons + n;
  return n;
}
