//
// Created by KiberPerdun on 15.12.2024.
//

#ifndef LIBKPNET_GET_RANDOM_H
#define LIBKPNET_GET_RANDOM_H

#include "types.h"
#include <sys/random.h>
#include <stdio.h>

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

u16
get_random_u16 ()
{
  u16 random;
  i64 res;

  /* GRND_NONBLOCK will be set EAGAIN if block */
  res = getrandom (&random, sizeof (u16), GRND_NONBLOCK | GRND_RANDOM);
  if (res == -1)
    {
      fputs ("Error, random gen", stderr);
      return 0;
    }

  if (res != sizeof (u16))
    {
      fputs ("Incomplete random data read", stderr);
      return 0;
    }

  return random;
}

u8
get_random_u8 ()
{
  u8 random;
  i64 res;

  /* GRND_NONBLOCK will be set EAGAIN if block */
  res = getrandom (&random, sizeof (u8), GRND_NONBLOCK | GRND_RANDOM);
  if (res == -1)
    {
      fputs ("Error, random gen", stderr);
      return 0;
    }

  if (res != sizeof (u8))
    {
      fputs ("Incomplete random data read", stderr);
      return 0;
    }

  return random;
}

u64
get_random_u64 ()
{
  u64 random;
  i64 res;

  /* GRND_NONBLOCK will be set EAGAIN if block */
  res = getrandom (&random, sizeof (u64), GRND_NONBLOCK | GRND_RANDOM);
  if (res == -1)
    {
      fputs ("Error, random gen", stderr);
      return 0;
    }

  if (res != sizeof (u64))
    {
      fputs ("Incomplete random data read", stderr);
      return 0;
    }

  return random;
}

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

#endif // LIBKPNET_GET_RANDOM_H
