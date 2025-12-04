//
// Created by KiberPerdun on 8/11/25.
//

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "types.h"
#include <stdatomic.h>
#include <stdio.h>
#include <sys/queue.h>

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

typedef u0 (*timer_callback) (u0);

typedef struct ringtimer_callback
{
  u64 retries;
  timer_callback callback;
  struct ringtimer_callback *prev;
  struct ringtimer_callback *next;
} ringtimer_callback_t;

typedef struct ringtimer
{
  ringtimer_callback_t **timers;
  u32 current;
  u32 max;
  ringbuf_t *allocator;
} ringtimer_t;

ringbuf_t *create_ringbuf (u64 size);
i64 push_ringbuf (ringbuf_t *rb, u0 *packet, u64 plen);
ringbuf_cell_t *pop_ringbuf (ringbuf_t *rb);
ringbuf_cell_t *free_ringbuf (ringbuf_t *rb);

ringbuf_t *create_allocator (u64 memory_size, u64 memory_num);

ringtimer_t *create_ringtimer (u32 timers_count, ringbuf_t *allocator);
ringtimer_t *insert_ringtimer (timer_callback callback, u32 time, ringtimer_t *ring);
ringtimer_t *tick_ringtimer (ringtimer_t *ring);

#endif //RING_BUFFER_H
