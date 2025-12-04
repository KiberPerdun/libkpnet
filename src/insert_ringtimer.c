//
// Created by KiberPerdun on 11/27/25.
//

#include "ring_buffer.h"
#include <malloc.h>

ringtimer_t *
insert_ringtimer (timer_callback callback, u32 time, ringtimer_t *ring)
{
  ringtimer_callback_t *first, *last, *cur;
  ringbuf_cell_t *cell;
  u32 slot;

  slot = ring->current + time;

  for (; (cell = pop_ringbuf (ring->allocator)) == 0;)
    ;

  if (!cell->packet)
    {
      push_ringbuf (ring->allocator, cell->packet, 0);
      return NULL;
    }

  if (!ring->timers[slot % ring->max])
    {
      ring->timers[slot % ring->max] = cell->packet;
      first = ring->timers[slot % ring->max];
      first->prev = first;
      first->next = first;
      first->callback = callback;
      first->retries = slot / ring->max;
    }
  else
    {
      first = ring->timers[slot % ring->max];
      last = first->prev;
      last->next = cell->packet;
      cur = last->next;
      cur->next = first;
      cur->prev = last;
      cur->callback = callback;
      cur->retries = slot / ring->max;
      first->prev = cur;
    }

  return ring;
}
