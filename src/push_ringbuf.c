//
// Created by KiberPerdun on 9/26/25.
//

#include "ring_buffer.h"

i64
push_ringbuf (ringbuf_t *rb, u0 *packet, u64 plen)
{
  ringbuf_cell_t *cell;
  u64 pos, seq;
  i64 diff;

  pos = atomic_load_explicit (&rb->head, memory_order_relaxed);
  for (;;)
    {
      cell = &rb->cells[pos & rb->mask];
      seq = atomic_load_explicit (&cell->seq, memory_order_acquire);
      diff = seq - pos;

      if (0 == diff)
        if (atomic_compare_exchange_weak_explicit (&rb->head, &pos, pos + 1,
                                                   memory_order_relaxed,
                                                   memory_order_relaxed))
          break;

      if (diff < 0)
        return -1;

      else
        pos = atomic_load_explicit (&rb->head, memory_order_relaxed);
    }

  cell->packet = packet;
  cell->plen = plen;
  atomic_store_explicit (&cell->seq, pos + 1, memory_order_release);
  return 0;
}
