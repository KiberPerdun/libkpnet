//
// Created by KiberPerdun on 8/11/25.
//

#include "ring_buffer.h"

#include <stdlib.h>

ring_buffer_t *
free_ring_buffer (ring_buffer_t *rb)
{
  if (!rb)
    return NULL;

  for (i16 i = 0; i < SIZE; ++i)
    free (rb->buffer[i]);

  free (rb);

  return rb;
}
