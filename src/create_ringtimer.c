//
// Created by KiberPerdun on 11/27/25.
//

#include "ring_buffer.h"
#include <malloc.h>

ringtimer_t *
create_ringtimer (u32 timers_count, ringbuf_t *allocator)
{
  ringtimer_t *timer;

  timer = malloc (sizeof (ringtimer_t));
  if (!timer)
    return NULL;

  timer->current = 0;
  timer->max = timers_count;
  timer->allocator = allocator;
  timer->timers = calloc (1, timers_count * sizeof (ringtimer_callback_t *));
  if (!timer->timers)
    {
      free (timer);
      return NULL;
    }

  return timer;
}
