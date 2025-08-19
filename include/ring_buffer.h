//
// Created by KiberPerdun on 8/11/25.
//

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "types.h"

#define SIZE 1024

typedef struct ring_buffer
{
  u0 *buffer[SIZE];
  _Atomic u32 consumer;
  _Atomic u32 productor;
} ring_buffer_t;

ring_buffer_t *init_ring_buffer ();
ring_buffer_t *free_ring_buffer (ring_buffer_t *rb);
ring_buffer_t *fill_ring_buffer (ring_buffer_t *rb, u64 buf_size);
u0 *get_next_address_ring_buffer_consumer (ring_buffer_t *rb);
u0 *get_next_address_ring_buffer_producer (ring_buffer_t *rb);

#endif //RING_BUFFER_H
