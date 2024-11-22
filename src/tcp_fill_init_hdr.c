//
// Created by KiberPerdun on 22.10.24.
//

#include "tcp.h"
#include <arpa/inet.h>
#include "if_packet.h"

bool
tcp_fill_init_hdr (u0 *ars)
{
  connection_args_t *args;
  tcp_t *hdr;

  if (args->packet == NULL)
    return false;

  if (!(MAX_PACKET_LEN >= args->plen + sizeof (tcp_t)))
    return false;

  if ((hdr = malloc (sizeof (tcp_t))) == NULL)
    {
      free (hdr);
      return false;
    }

  args = ars;

  hdr->srcp = args->srcport;
  hdr->dstp = args->dstport;
  hdr->seq = 0;
  hdr->ack = 0;
  hdr->flags = htons ((5 + (4 / 4)) << 12 | 0x011);
  hdr->win = htons (64240);
  hdr->check = 0;
  hdr->urgent = 0;
  return true;
}
