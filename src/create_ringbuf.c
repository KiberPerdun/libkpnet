//
// Created by KiberPerdun on 9/26/25.
//

#include "ring_buffer.h"
#include <malloc.h>

ringbuf_t *
create_ringbuf (u64 size)
{
  ringbuf_t *rb;
  if (!((rb = malloc (sizeof (ringbuf_t)))))
    {
      free (rb);
      return NULL;
    }

  rb->size = size;
  rb->mask = rb->size - 1;
  if (!((rb->cells = malloc (sizeof (ringbuf_t) * rb->size))))
    {
      free (rb);
      return NULL;
    }
  atomic_init (&rb->head, 0);
  atomic_init (&rb->tail, 0);

  for (i64 i = 0; i < size; ++i)
    {
      atomic_init (&rb->cells[i].seq, i);
      rb->cells[i].packet = NULL;
    }

  return rb;
}
