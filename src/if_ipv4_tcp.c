//
// Created by KiberPerdun on 14.11.24.
//

#include "if_packet.h"

bool
if_ipv4_tcp (u0 *packet, u64 size, packet_args_t *args)
{
  if (if_ipv4 (packet, size, args))
    if (if_tcp (packet, size - 14, args))
      return true;

  return false;
}
