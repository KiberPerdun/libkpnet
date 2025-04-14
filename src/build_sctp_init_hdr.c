//
// Created by KiberPerdun on 3/14/25.
//

#include "if_packet.h"

frame_data_t *
build_sctp_init_hdr (frame_data_t *frame, u32 tag, u32 a_rwnd, u16 os, u16 mis, u32 tsn)
{
  frame = build_sctp_fld_hdr_raw (frame, 1, 0, 20);

  if (frame->plen < sizeof (sctp_init_hdr_t) || NULL == frame->packet)
    {
      frame->plen = 0;
      return frame;
    }

  sctp_init_hdr_t *hdr;
  hdr = frame->packet;

  hdr->init_tag = htonl (tag);
  hdr->a_rwnd = htonl (a_rwnd);
  hdr->os = htons (os);
  hdr->mis = htons (mis);
  hdr->init_tsn = htonl (tsn);

  frame->packet += sizeof (sctp_init_hdr_t);
  frame->plen -= sizeof (sctp_init_hdr_t);

  return frame;
}

