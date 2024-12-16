//
// Created by KiberPerdun on 11.12.2024.
//

#include "if_packet.h"

bool
if_ipv4_sctp (u0 *packet, u64 size, connection_args_t *args)
{
  if (if_ipv4 (packet, size, args))
    if (if_sctp (packet + sizeof (mac_t) + sizeof (ipv4_t), size - sizeof (mac_t) - sizeof (ipv4_t), args))
      return true;

  return false;
}
