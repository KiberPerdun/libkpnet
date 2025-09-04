//
// Created by KiberPerdun on 9/4/25.
//

#include "if_packet.h"

frame_data_t *
prefill_mac_ip_sctp_ (frame_data_t *frame)
{
  if_ip_sctp_meta_t *meta;

  if (__builtin_expect (frame == NULL, 0) ||  __builtin_expect (frame->prefill == NULL || frame->state == NULL, 0))
    return NULL;

  meta = frame->state;

  frame->prefill += sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t);
  frame->prefill = build_sctp_cmn_hdr_raw (frame->prefill, &frame->plen, meta->src_port, meta->dst_port, 0);
  frame->prefill = build_ip_raw (frame->prefill, &frame->plen, meta->src_ip, meta->dst_ip, PROTO_SCTP);
  frame->prefill = build_mac_raw (frame->prefill, &frame->plen, meta->gateway, meta->dev, ETHERTYPE_IP);

  return frame;
}
