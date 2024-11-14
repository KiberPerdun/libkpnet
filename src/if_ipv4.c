//
// Created by KiberPerdun on 14.11.24.
//

#include "if_packet.h"

bool
if_ipv4 (u0 *packet, u64 size, packet_args_t *args)
{
  ipv4_t *hdr;

  if (args->net_layer.ipv4.ver == 4)
  {
    hdr = packet + 14;

    if (hdr->src_addr == args->net_layer.ipv4.dest_addr)
      {
        printf ("[Packet start\n{ipv4}(proto: %d, len: %d);\n",
                hdr->proto,
                ntohs (hdr->len));
        return true;
      }
  }

  return false;
}
