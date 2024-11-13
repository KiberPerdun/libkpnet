//
// Created by KiberPerdun on 30.10.24.
//

#include "tcp.h"

u0 *
build_tcp_raw (u16 src_port, u16 dst_port, u32 seq, u32 ack, u16 flags,
               u16 win, u16 urgent, u8 optlen)
{
  tcp_t *hdr;

  if (!(hdr = malloc (sizeof (tcp_t) + optlen)))
    return NULL;

  hdr->srcp = htons (src_port);
  hdr->dstp = htons (dst_port);
  hdr->check = 0;
  hdr->urgent = htons (urgent);
  hdr->win = htons (win);
  hdr->flags = htons ((5 + (optlen / 4)) << 12 | flags);
  hdr->seq = htonl (seq);
  hdr->ack = htonl (ack);

  return hdr;
}
