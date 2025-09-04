//
// Created by KiberPerdun on 9/4/25.
//

#include "checks.h"
#include "prefilled.h"
#include "random.h"

frame_data_t *build_prefilled_mac_ip_sctp_init_hdr (frame_data_t *frame)
{
  if_ip_sctp_meta_t *meta;
  sctp_cmn_hdr_t *cmn;
  u0 *packet;
  ipv4_t *ip;

  if (__builtin_expect (frame == NULL, 0) ||  __builtin_expect (frame->prefill == NULL || frame->state == NULL, 0))
    return NULL;

  meta = frame->state;
  packet = frame->prefill;
  frame->prefill += sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t) + sizeof (sctp_fld_hdr_t) + sizeof (sctp_init_hdr_t);
  frame->prefill = build_sctp_init_hdr_raw (frame->prefill, &frame->plen, get_random_from_buffer_u32 (), meta->src_arwnd, meta->src_os, meta->src_mis, get_random_from_buffer_u32 ());
  frame->prefill = build_sctp_fld_hdr_raw (frame->prefill, &frame->plen, SCTP_INIT, 0, sizeof (sctp_init_hdr_t) + sizeof (sctp_fld_hdr_t));
  cmn = frame->prefill - sizeof (sctp_cmn_hdr_t);
  ip = (u0 *) cmn - sizeof (ipv4_t);
  ip->len = htons (frame->plen - sizeof (mac_t));
  ip->check += htons (0) - ip->len;
  if (ip->check > 0xFFFF)
    ip->check = (ip->check & 0xFFFF) + (ip->check >> 16);

  cmn->check = generate_crc32c ((const u8 *) cmn, sizeof (sctp_cmn_hdr_t ) + sizeof (sctp_fld_hdr_t) + sizeof (sctp_init_hdr_t));

  frame->prefill = packet;
  return frame;
}
