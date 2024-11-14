//
// Created by KiberPerdun on 14.11.24.
//

#include "if_packet.h"

bool
if_tcp (u0 *packet, u64 size, packet_args_t *args)
{
  tcp_t *buf;

  if (size >= sizeof (tcp_t) + sizeof (tcp_opt_mss_t))
    {

      if (args->net_layer.ipv4.ver == 4)
        buf = packet + 14 + sizeof (ipv4_t);

      else if (args->net_layer.ipv4.ver == 6)
        buf = packet + 14 + sizeof (ipv6_t);

      else
        return false;

      printf ("{tcp}(seq: %u, ack: %u, win: %hu, urgent: %hu);\nPacket end]\n",
              ntohl (buf->seq), ntohl (buf->ack), ntohs (buf->win), ntohs (buf->urgent));
      fflush (stdout);
    }

  return false;
}
