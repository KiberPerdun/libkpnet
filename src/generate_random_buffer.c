//
// Created by KiberPerdun on 8/14/25.
//

#include "random.h"

u8 random_buffer[RANDOM_BUFFER_SIZE] = { 0 };
_Atomic u32 random_buffer_head = 0;

u0
generate_random_buffer ()
{
  random_buffer_head = 0;
  get_random (random_buffer, sizeof (random_buffer));
}
