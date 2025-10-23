//
// Created by KiberPerdun on 10/17/25.
//

#include "ring_buffer.h"
#include <malloc.h>

timeouts_t *
create_timeouts (u64 hz)
{
  timeouts_t *times;
  if (!(times = malloc (sizeof (timeouts_t))))
    return NULL;

  for (i32 i = 0; i < countof (times->wheel); ++i)
    for (i32 j = 0; i < countof (times->wheel[i]); ++j)
      TAILQ_INIT (&times->wheel[i][j]);

  TAILQ_INIT (&times->expired);
  for (i32 i = 0; i < countof (times->pending); ++i)
    times->pending[i] = 0;

  times->cur = 0;
  times->hz = hz;

  return times;
}
