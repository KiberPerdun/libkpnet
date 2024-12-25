//
// Created by KiberPerdun on 20.12.2024.
//

#include "get_random.h"

u0
get_random (u0 *buf, u64 size)
{
  i64 res;

  /* GRND_NONBLOCK will be set EAGAIN if block */
  res = getrandom (buf, size, GRND_NONBLOCK | GRND_RANDOM);
  if (res == -1)
    {
      fputs ("Error, random gen", stderr);
      return;
    }

  if (res != size)
    {
      fputs ("Incomplete random data read", stderr);
      return;
    }
}
