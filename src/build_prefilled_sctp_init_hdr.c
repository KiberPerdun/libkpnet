//
// Created by KiberPerdun on 9/4/25.
//

#include "checks.h"
#include "if_packet.h"
#include "random.h"

__attribute__ ((flatten)) frame_data_t *
build_prefilled_sctp_init_hdr (frame_data_t *frame)
{
  if_ip_sctp_meta_t *meta;
  sctp_cmn_hdr_t *cmn;
  u32 pck_len;
  ipv4_t *ip;
  u0 *packet;
  u16 len;

  if (__builtin_expect (frame == NULL, 0) ||  __builtin_expect (frame->prefill == NULL || frame->state == NULL, 0))
    return NULL;

  pck_len = frame->plen;
  frame->plen = 0;
  packet = frame->prefill;
  meta = frame->state;
  frame->prefill += sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t) + sizeof (sctp_fld_hdr_t) + sizeof (sctp_init_hdr_t);
  frame->prefill = build_sctp_init_hdr_raw (frame->prefill, &frame->plen, get_random_from_buffer_u32 (), meta->src_arwnd, meta->src_os, meta->src_mis, get_random_from_buffer_u32 ());
  frame->prefill = build_sctp_fld_hdr_raw (frame->prefill, &frame->plen, SCTP_INIT, 0, frame->plen + sizeof (sctp_fld_hdr_t));
  ip = packet + sizeof (mac_t);
  cmn = (u0 *) ip + sizeof (ipv4_t);

  /*
  ((ipv4_t *) packet)->check = ~(u16) ip_checksum ((u16 *) &hdr, sizeof (ipv4_t));
  */

  frame->plen += pck_len;
  len = ip->len;
  ip->len = ntohs (frame->plen - sizeof (mac_t));
  ip->check += len - ntohs (frame->plen - sizeof (mac_t));
  if (ip->check > 0xFFFF)
    ip->check = (ip->check & 0xFFFF) + (ip->check >> 16);

  cmn->check = 0;
  cmn->check = generate_crc32c ((const u8 *) cmn, frame->plen - sizeof (mac_t) - sizeof (ipv4_t));

  return frame;
}
