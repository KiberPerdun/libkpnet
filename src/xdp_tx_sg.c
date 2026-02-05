//
// Created by KiberPerdun on 10.01.26.
//

#include "xdp.h"

u32
xdp_tx_sg (xdp_t *xsk, xdp_sg_req_t *reqs, u32 n)
{
  u32 prod, cons, free_space, i, idx;
  xdp_desc_t *desc;

  prod = *xsk->tx_ring_producer;
  cons = *xsk->tx_ring_consumer;
  free_space = UMEM_CHUNK_COUNT - (prod - cons);

  if (n > free_space)
    n = free_space;

  if (0 == n)
    return 0;

  for (i = 0; i < n; ++i)
    {
      idx = (prod + i) & RING_MASK;
      xsk->tx_ring[idx].addr = reqs[i].addr;
      xsk->tx_ring[idx].len = reqs[i].len;
      xsk->tx_ring[idx].options = reqs[i].flags;
    }

  ring_smp_wmb ();
  *xsk->tx_ring_producer = prod + n;

  if (sendto (xsk->fd, NULL, 0, MSG_DONTWAIT, NULL, 0) < 0) {
      if (errno != EBUSY && errno != EAGAIN)
        /* TODO!: Обработать ошибку. */
          perror("sendto failed");
  }
  return n;
}
