//
// Created by KiberPerdun on 20.12.2024.
//

#include "get_random.h"

u32
get_random_u32 ()
{
  u32 random;
  i64 res;

  /* GRND_NONBLOCK will be set EAGAIN if block */
  res = getrandom (&random, sizeof (u32), GRND_NONBLOCK | GRND_RANDOM);
  if (res == -1)
    {
      fputs ("Error, random gen", stderr);
      return 0;
    }

  if (res != sizeof (u32))
    {
      fputs ("Incomplete random data read", stderr);
      return 0;
    }

  return random;
}
