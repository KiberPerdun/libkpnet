//
// Created by KiberPerdun on 7/20/25.
//

#include "if_packet.h"

frame_data_t *
build_sctp_cookie_echo_hdr (frame_data_t *frame, u0 *meta)
{
  if_ip_sctp_meta_t *m = meta;

  if (frame->plen < 4 + m->add_len || NULL == frame->packet)
    {
      frame->plen = 0;
      return frame;
    }

  //frame = build_sctp_fld_hdr_raw1 (frame, 10, 0, 4 + m->add_len);
  memcpy (frame->packet, m->add, m->add_len);

  frame->packet += m->add_len;
  frame->plen -= m->add_len;

  return frame;
}
