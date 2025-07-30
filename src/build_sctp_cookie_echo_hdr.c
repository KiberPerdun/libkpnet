//
// Created by KiberPerdun on 7/20/25.
//

#include "get_random.h"
#include "if_packet.h"

frame_data_t *
build_sctp_cookie_echo_hdr (frame_data_t *frame)
{
  if_ip_sctp_meta_t *meta;

  if (NULL == frame || NULL == frame->packet || NULL == frame->state)
    return NULL;

  meta = frame->state;

  frame->packet += sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t) + sizeof (sctp_fld_hdr_t) + sizeof (sctp_cookie_echo_hdr_t) + meta->add_len;
  frame->packet = build_sctp_cookie_echo_hdr_raw (frame->packet, &frame->plen, meta->add, meta->add_len);
  frame->packet = build_sctp_fld_hdr_raw (frame->packet, &frame->plen, SCTP_COOKIE_ECHO, 0, frame->plen + sizeof (sctp_fld_hdr_t));
  frame->packet = build_sctp_cmn_hdr_raw (frame->packet, &frame->plen, meta->src_port, meta->dst_port, meta->init_tag);
  frame->packet = build_ip_raw (frame->packet, &frame->plen, meta->src_ip, meta->dst_ip, PROTO_SCTP);
  frame->packet = build_mac_raw (frame->packet, &frame->plen, meta->gateway, meta->dev, ETHERTYPE_IP);

  free (meta->add);

  return frame;
}
