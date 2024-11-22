//
// Created by KiberPerdun on 30.10.24.
//

#include "tcp.h"
#include "if_packet.h"

u0 *
build_tcp_raw (u32 seq, u32 ack, u16 flags,
               u16 win, u16 urgent, u8 optlen, u0 *options, u0 *ars)
{
  connection_args_t *args;
  tcp_t *hdr;

  args = ars;

  if (!(hdr = malloc (sizeof (tcp_t) + optlen)))
    return NULL;

  hdr->srcp = args->srcport;
  hdr->dstp = args->dstport;
  hdr->check = 0;
  hdr->urgent = htons (urgent);
  hdr->win = htons (win);
  hdr->flags = htons ((5 + (optlen / 4)) << 12 | flags);
  hdr->seq = htonl (seq);
  hdr->ack = htonl (ack);

  return hdr;
}
