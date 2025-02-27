//
// Created by KiberPerdun on 2/27/25.
//

#include "checks.h"

u32
ip_checksum (const u16 *data, u16 len)
{
  u32 sum = 0;
  for (u16 i = 0; i < len >> 1; ++i)
    {
      sum += data[i];
      if (sum > 0xFFFF)
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

  return sum;
}
