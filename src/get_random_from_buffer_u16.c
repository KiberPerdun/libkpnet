//
// Created by KiberPerdun on 8/14/25.
//

#include "random.h"

inline __attribute__ ((always_inline, hot)) u16
get_random_from_buffer_u16 ()
{
  if (sizeof (random_buffer) - random_buffer_head >= sizeof (u16))
    {
      u8 *buf = random_buffer + random_buffer_head;
      random_buffer_head += sizeof (u16);
      return *(u16 *) buf;
    }

  generate_random_buffer ();
  return get_random_from_buffer_u16 ();
}
