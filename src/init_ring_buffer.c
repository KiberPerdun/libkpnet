//
// Created by KiberPerdun on 8/11/25.
//

#include "ring_buffer.h"

#include <stdlib.h>
#include <string.h>

ring_buffer_t *
init_ring_buffer ()
{
  ring_buffer_t *rb;

  if (!((rb = malloc (sizeof (ring_buffer_t)))))
    return NULL;

  memset (rb->buffer, 0, sizeof (rb->buffer));

  rb->head = 0;
  rb->tail = 0;

  return rb;
}
