//
// Created by KiberPerdun on 10.01.26.
//

#include "if_packet.h"
#include "xdp.h"

u32
xdp_tx (xdp_t *xsk, u0 **xdp_segs, const u32 *xdp_seg_lens, u32 n)
{
  u32 prod, cons, diff, idx;

  if (0 == n || n > XDP_MAX_SEGS)
    return 0;

  prod = *xsk->tx_ring_producer;
  cons = *xsk->tx_ring_consumer;
  diff = UMEM_CHUNK_COUNT - (prod - cons);

  if (n > diff)
    return 0;

  for (i32 i = 0; i < n; ++i)
    {
      idx = (prod + i) & RING_MASK;

      xsk->tx_ring[idx].addr = (u64) xdp_segs[i];
      printf ("%lu", (u64) xdp_segs[i]);
      puts (" ");
      xsk->tx_ring[idx].len = xdp_seg_lens[i];

      if (i < n - 1)
        xsk->tx_ring[idx].options = XDP_PKT_CONTD;

      else
        xsk->tx_ring[idx].options = 0;
    }

  ring_smp_wmb ();
  *xsk->tx_ring_producer = prod + n;

  if (sendto (xsk->fd, NULL, 0, MSG_DONTWAIT, NULL, 0) < 0)
    {
      if (errno != EBUSY && errno != EAGAIN)
        perror("sendto failed");

    }

  return n;
}