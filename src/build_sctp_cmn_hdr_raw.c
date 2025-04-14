//
// Created by KiberPerdun on 3/14/25.
//

#include "if_packet.h"

frame_data_t *
build_sctp_cmn_hdr_raw (frame_data_t *frame, u16 srcport, u16 dstport, u32 tag)
{
  if (frame->plen < sizeof (sctp_cmn_hdr_t) || NULL == frame->packet)
    {
      frame->plen = 0;
      return frame;
    }

  sctp_cmn_hdr_t *hdr;
  hdr = frame->packet;

  hdr->check = 0;
  hdr->srcp = htons (srcport);
  hdr->dstp = htons (dstport);
  hdr->tag = htonl (tag);

  frame->packet += sizeof (sctp_cmn_hdr_t);
  frame->plen -= sizeof (sctp_cmn_hdr_t);

  return frame;
}

