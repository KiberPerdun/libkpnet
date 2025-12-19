//
// Created by KiberPerdun on 11/27/25.
//

#include "ring_buffer.h"
#include <malloc.h>

timer_tick_result_t
tick_ringtimer (ringtimer_t *ring)
{
  u32 current = ring->current;
  timer_tick_result_t res;
  res.count = 0;
  ringtimer_callback_t *head = ring->timers[current];

  if (head)
    {
      ringtimer_callback_t *t = head;
      do
        {
          ringtimer_callback_t *next = t->next;

          if (t->signal)
            {
              if (t->retries == 0)
                {
                  res.signals[res.count++] = t->signal;

                  if (t->next == t)
                    ring->timers[current] = NULL;

                  else
                    {
                      t->prev->next = t->next;
                      t->next->prev = t->prev;

                      if (ring->timers[current] == t)
                        ring->timers[current] = t->next;

                    }

                  push_ringbuf (ring->allocator, t, 64);
                }
              else
                t->retries--;

            }

          if (!ring->timers[current])
            break;

          t = next;
        }
      while (t != head);
    }

  ring->current = (ring->current + 1) % ring->max;
  return res;
}
