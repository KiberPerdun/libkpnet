//
// Created by KiberPerdun on 3/14/25.
//

#include "if_packet.h"

frame_data_t *
build_sctp_fld_hdr_raw (frame_data_t *frame, u8 type, u8 flags, u16 len)
{
  if (frame->plen < sizeof (sctp_fld_hdr_t) || NULL == frame->packet)
    {
      frame->plen = 0;
      return frame;
    }

  sctp_fld_hdr_t *hdr;
  hdr = frame->packet;

  hdr->type = type;
  hdr->flags = flags;
  hdr->len = htons (len);

  frame->packet += sizeof (sctp_fld_hdr_t);
  frame->plen -= sizeof (sctp_fld_hdr_t);

  return frame;
}

