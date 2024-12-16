//
// Created by KiberPerdun on 14.11.24.
//

#include "if_packet.h"

bool
if_ipv4 (u0 *packet, u64 size, connection_args_t *args)
{
  ipv4_t *hdr;

  if (sizeof (mac_t) + sizeof (ipv4_t) <= size)
  {
    hdr = packet + sizeof (mac_t);

    /* will be deprecated */
    if (hdr->src_addr == args->net_layer.ipv4->dest_addr)
      {
        printf ("[Packet start\n{ipv4}(proto: %d, len: %d);\n",
                hdr->proto,
                ntohs (hdr->len));
        return true;
      }
  }

  return false;
}
