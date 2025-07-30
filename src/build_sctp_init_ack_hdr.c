//
// Created by KiberPerdun on 4/28/25.
//

#include "get_random.h"
#include "if_packet.h"

frame_data_t *
build_sctp_init_ack_hdr (frame_data_t *frame)
{
  if_ip_sctp_meta_t *meta;

  if (NULL == frame || NULL == frame->packet || NULL == frame->state)
    return NULL;

  meta = frame->state;
  meta->init_tag = get_random_u32 ();
  meta->tsn = get_random_u32 ();

  frame->packet += sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t) + sizeof (sctp_fld_hdr_t) + sizeof (sctp_init_ack_hdr_t) + sizeof (sctp_cookie_t) + sizeof (cookie_t) + sizeof (u32);
  frame->packet = build_sctp_cookie_param_raw (frame->packet, &frame->plen, (cookie_t *) meta);
  frame->packet = build_sctp_init_ack_hdr_raw (frame->packet, &frame->plen, meta->init_tag, meta->src_arwnd, meta->src_os, meta->src_mis, meta->tsn);
  frame->packet = build_sctp_fld_hdr_raw (frame->packet, &frame->plen, SCTP_INIT_ACK, 0, frame->plen + sizeof (sctp_fld_hdr_t));
  frame->packet = build_sctp_cmn_hdr_raw (frame->packet, &frame->plen, meta->src_port, meta->dst_port, meta->ver_tag);
  frame->packet = build_ip_raw (frame->packet, &frame->plen, meta->src_ip, meta->dst_ip, PROTO_SCTP);
  frame->packet = build_mac_raw (frame->packet, &frame->plen, "libkpnet_s", "libkpnet_c", ETHERTYPE_IP);

  return frame;
}
