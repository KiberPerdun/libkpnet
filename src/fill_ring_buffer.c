//
// Created by KiberPerdun on 8/18/25.
//

#include "ring_buffer.h"

#include <stdio.h>
#include <stdlib.h>

ring_buffer_t *
fill_ring_buffer (ring_buffer_t *rb, u64 buf_size)
{
  for (i32 i = 0; i < SIZE; ++i)
  {
    rb->buffer[i] = aligned_alloc (64, buf_size + 63 & ~63);
    if (!rb->buffer[i])
    {
      perror ("Failed to allocate memory for ring buffer");
      return NULL;
    }
  }

  return rb;
}
