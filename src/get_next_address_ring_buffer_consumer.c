//
// Created by KiberPerdun on 8/18/25.
//

#include "ring_buffer.h"

#include <stddef.h>
#include <stdio.h>
#include <xmmintrin.h>

u0 *
get_next_address_ring_buffer_consumer (ring_buffer_t *rb)
{
  if (rb->productor == rb->consumer)
    return NULL;

  u32 current = rb->consumer;
  u32 next = (current + 1) % SIZE;

  _mm_prefetch ((u8 *) rb->buffer[next], _MM_HINT_T0);
  rb->consumer = next;
  return rb->buffer[current];
}
