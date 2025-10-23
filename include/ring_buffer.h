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


typedef struct timeout
{
  u64 expires;
  u64 interval;

  struct timeout_list *pending;
  TAILQ_ENTRY (timeout) tqe;

  struct {
    u0 (*fn) ();
    u0 *args;
  } callback;

  struct timeouts *timeouts;
} timeout_t;

#define WHEEL_NUM 3
#define WHEEL_LEN 64
TAILQ_HEAD (timeout_list, timeout);

typedef struct timeouts {
  struct timeout_list wheel[WHEEL_NUM][WHEEL_LEN], expired;

  u64 pending[WHEEL_NUM];

  u64 cur;
  u64 hz;
} timeouts_t;
#define countof(a) (sizeof (a) / sizeof *(a))

ringbuf_t *create_ringbuf (u64 size);
i64 push_ringbuf (ringbuf_t *rb, u0 *packet, u64 plen);
ringbuf_cell_t *pop_ringbuf (ringbuf_t *rb);

timeouts_t *create_timeouts (u64 hz);
u0 add_timeouts (timeouts_t *times, timeout_t *to, u64 timeout);

#endif //RING_BUFFER_H
