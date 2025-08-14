//
// Created by KiberPerdun on 8/14/25.
//

#include "random.h"

inline __attribute__ ((always_inline, hot)) u32
get_random_from_buffer_u32 ()
{
  if (sizeof (random_buffer) - random_buffer_head >= sizeof (u32))
    {
      u8 *buf = random_buffer + random_buffer_head;
      random_buffer_head += sizeof (u32);
      return *(u32 *) buf;
    }

  generate_random_buffer ();
  return get_random_from_buffer_u32 ();
}
