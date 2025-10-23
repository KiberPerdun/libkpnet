//
// Created by KiberPerdun on 10/17/25.
//

#include "ring_buffer.h"

u0
add_timeouts (timeouts_t *times, timeout_t *to, u64 timeout)
{
  to->interval = timeout;
}
