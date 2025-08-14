//
// Created by KiberPerdun on 8/14/25.
//

#include "random.h"

inline __attribute__ ((always_inline, hot)) u64
get_random_from_buffer_u64 ()
{
  if (sizeof (random_buffer) - random_buffer_head >= sizeof (u64))
    {
      u8 *buf = random_buffer + random_buffer_head;
      random_buffer_head += sizeof (u64);
      return *(u64 *) buf;
    }

  generate_random_buffer ();
  return get_random_from_buffer_u64 ();
}
