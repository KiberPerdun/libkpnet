//
// Created by KiberPerdun on 9/26/25.
//

#include "ring_buffer.h"

ringbuf_cell_t *pop_ringbuf (ringbuf_t *rb)
{
  ringbuf_cell_t *cell;
  u64 pos, seq;
  i64 diff;

  pos = atomic_load_explicit (&rb->tail, memory_order_relaxed);
  for (;;)
    {
      cell = &rb->cells[pos & rb->mask];
      seq = atomic_load_explicit (&cell->seq, memory_order_acquire);
      diff = seq - (pos + 1);

      if (0 == diff)
        if (atomic_compare_exchange_weak_explicit (&rb->tail, &pos, pos + 1,
                                                   memory_order_relaxed,
                                                   memory_order_relaxed))
          break;

      if (diff < 0)
        return NULL;

      else
        pos = atomic_load_explicit (&rb->tail, memory_order_relaxed);
    }

  atomic_store_explicit (&cell->seq, pos + rb->mask + 1, memory_order_release);
  return cell;
}
