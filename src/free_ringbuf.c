//
// Created by KiberPerdun on 11/27/25.
//

#include "ring_buffer.h"
#include <malloc.h>

ringbuf_cell_t *
free_ringbuf (ringbuf_t *rb)
{
 for (i64 i = 0; i < rb->size; ++i)
   free (rb->cells[i].packet);

 free (rb->cells);
 free (rb);

  return NULL;
}
