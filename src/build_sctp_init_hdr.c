//
// Created by KiberPerdun on 3/14/25.
//

#include "if_packet.h"
#include "random.h"

__attribute__ ((flatten)) frame_data_t *
build_sctp_init_hdr (frame_data_t *frame)
{
  if_ip_sctp_meta_t *meta;

  if (__builtin_expect (frame == NULL, 0) ||  __builtin_expect (frame->packet == NULL || frame->state == NULL, 0))
    return NULL;

  meta = frame->state;
  frame->packet += sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t) + sizeof (sctp_fld_hdr_t) + sizeof (sctp_init_hdr_t);
  frame->packet = build_sctp_init_hdr_raw (frame->packet, &frame->plen, get_random_from_buffer_u32 (), meta->src_arwnd, meta->src_os, meta->src_mis, get_random_from_buffer_u32 ());
  frame->packet = build_sctp_fld_hdr_raw (frame->packet, &frame->plen, SCTP_INIT, 0, frame->plen + sizeof (sctp_fld_hdr_t));
  frame->packet = build_sctp_cmn_hdr_raw (frame->packet, &frame->plen, meta->src_port, meta->dst_port, 0);
  frame->packet = build_ip_raw (frame->packet, &frame->plen, meta->src_ip, meta->dst_ip, PROTO_SCTP);
  frame->packet = build_mac_raw (frame->packet, &frame->plen, meta->gateway, meta->dev, ETHERTYPE_IP);

  return frame;
}
