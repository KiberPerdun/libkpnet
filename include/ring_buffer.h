//
// Created by KiberPerdun on 8/11/25.
//

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "types.h"
#include <stdatomic.h>
#include <stdio.h>

typedef struct ringbuf_cell
{
  _Atomic u64 seq;
  _Atomic u64 plen;
  u0 *packet;
} ringbuf_cell_t;

typedef struct ringbuf
{
  u64 size;
  u64 mask;
  _Atomic u64 head;
  _Atomic u64 tail;
  ringbuf_cell_t *cells;
} ringbuf_t;

ringbuf_t *create_ringbuf (u64 size);
i64 push_ringbuf (ringbuf_t *rb, u0 *packet, u64 plen);
ringbuf_cell_t *pop_ringbuf (ringbuf_t *rb);

#endif //RING_BUFFER_H
