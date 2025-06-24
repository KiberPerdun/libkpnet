//
// Created by KiberPerdun on 4/28/25.
//

#include "if_packet.h"
#include "get_random.h"

frame_data_t *
build_sctp_init_ack_hdr (frame_data_t *frame, u32 tag, u32 a_rwnd, u16 os, u16 mis, u32 tsn, u0 *meta)
{
  frame = build_sctp_fld_hdr_raw (frame, 2, 0, 28);

  if (frame->plen < sizeof (sctp_init_ack_hdr_t) || NULL == frame->packet)
    {
      frame->plen = 0;
      return frame;
    }

  if_ip_sctp_meta_t *m = meta;

  sctp_init_ack_hdr_t *hdr;
  hdr = frame->packet;

  hdr->init_tsn = m->src_tsn = get_random_u32 ();
  hdr->init_tag = m->dst_ver_tag = get_random_u32 ();
  hdr->a_rwnd = htonl (a_rwnd);
  hdr->os = htons (os);
  hdr->mis = htons (mis);
  hdr->cookie.len = htons (8);
  hdr->cookie.type = htons (7);
  *(u32 *)(((u0 *)&(hdr->cookie)) + 4) = htons (1234);

  frame->packet += sizeof (sctp_init_ack_hdr_t) + 4; /* 4 cookie size */
  frame->plen -= sizeof (sctp_init_ack_hdr_t) + 4;

  return frame;
}
