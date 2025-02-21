//
// Created by KiberPerdun on 30.10.24.
//

#include "tcp.h"
#include "if_packet.h"

frame_data_t *
build_tcp_raw (frame_data_t *frame, u16 srcp, u16 dstp, u32 seq, u32 ack, u16 flags,
               u16 win, u16 urgent, u8 optlen)
{
  if (frame->plen < sizeof (tcp_t) | NULL == frame->packet)
    {
      frame->plen = 0;
      return frame;
    }

  tcp_t *hdr;

  hdr = frame->packet;

  hdr->srcp = srcp;
  hdr->dstp = dstp;
  hdr->check = 0;
  hdr->urgent = htons (urgent);
  hdr->win = htons (win);
  hdr->flags = htons ((5 + (optlen / 4)) << 12 | flags);
  hdr->seq = htonl (seq);
  hdr->ack = htonl (ack);

  frame->packet += sizeof (tcp_t);
  frame->plen -= sizeof (tcp_t);

  return frame;
}
