//
// Created by KiberPerdun on 8/18/25.
//

#include "ring_buffer.h"

#include <stddef.h>
#include <stdio.h>
#include <xmmintrin.h>

u0 *
get_next_address_ring_buffer_producer (ring_buffer_t *rb)
{
  u32 next = (rb->productor + 1) % SIZE;

  if (next == rb->consumer)
    return NULL;

  u32 current = rb->productor;

  //printf ("%d, %d, %d\n", rb->consumer, next, rb->productor);

  _mm_prefetch ((u8 *) rb->buffer[next], _MM_HINT_T0);
  rb->productor = next;
  return rb->buffer[current];
}
