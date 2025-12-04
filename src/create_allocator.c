//
// Created by KiberPerdun on 11/27/25.
//

#include "hwinfo.h"
#include "ring_buffer.h"
#include <malloc.h>
#include <stdlib.h>

ringbuf_t *
create_allocator (u64 memory_size, u64 memory_num)
{
  ringbuf_cell_t *cell;
  ringbuf_t *rb;
  u0 *memory;
  if (!((rb = malloc (sizeof (ringbuf_t)))))
    {
      free (rb);
      return NULL;
    }

  rb->size = memory_num;
  rb->mask = rb->size - 1;
  if (!((rb->cells = malloc (sizeof (ringbuf_t) * rb->size))))
    {
      free (rb);
      return NULL;
    }
  atomic_init (&rb->head, 0);
  atomic_init (&rb->tail, 0);

  for (i64 i = 0; i < rb->size; ++i)
      atomic_init (&rb->cells[i].seq, i);

  for (i64 i = 0; i < rb->size; ++i)
    {
      memory = aligned_alloc (CACHELINE_SIZE, memory_size + (CACHELINE_SIZE - 1) & ~(CACHELINE_SIZE - 1));
      if (!memory)
        {
          cell = pop_ringbuf (rb);
          for (; cell; cell = pop_ringbuf (rb))
            {
              free (cell->packet);
              free (cell);
            }
          free (memory);
        }

      push_ringbuf (rb, memory, memory_size + (CACHELINE_SIZE - 1) & ~(CACHELINE_SIZE - 1));
    }

  return rb;
}

