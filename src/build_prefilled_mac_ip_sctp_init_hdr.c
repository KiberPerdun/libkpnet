//
// Created by KiberPerdun on 9/4/25.
//

#include "checks.h"
#include "prefilled.h"
#include "random.h"

frame_data_t *
build_prefilled_mac_ip_sctp_init_hdr (frame_data_t *frame)
{
  if_ip_sctp_meta_t *meta;
  sctp_cmn_hdr_t *cmn;
  u0 *packet;
  ipv4_t *ip;
  u32 chk;

  struct
  {
    sctp_fld_hdr_t fld;
    sctp_init_hdr_t init;
  } hdr;

  if (__builtin_expect (frame == NULL, 0) ||  __builtin_expect (frame->prefill == NULL || frame->state == NULL, 0))
    return NULL;

  meta = frame->state;
  packet = frame->prefill;
  frame->prefill += sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t) + sizeof (sctp_fld_hdr_t) + sizeof (sctp_init_hdr_t);

  frame->prefill -= (sizeof (sctp_init_hdr_t) + sizeof (sctp_fld_hdr_t));
  hdr.init.init_tag = get_random_from_buffer_u32 ();
  hdr.init.a_rwnd = htonl (meta->src_arwnd);
  hdr.init.os = htons (meta->src_os);
  hdr.init.mis = htons (meta->mis);
  hdr.init.init_tsn = htonl (meta->os);

  hdr.fld.type = SCTP_INIT;
  hdr.fld.flags = 0;
  hdr.fld.len = htons (sizeof (sctp_init_hdr_t) + sizeof (sctp_fld_hdr_t));

  memcpy (frame->prefill, &hdr, sizeof (sctp_init_hdr_t) + sizeof (sctp_fld_hdr_t ));
  frame->plen += sizeof (sctp_init_hdr_t) + sizeof (sctp_fld_hdr_t );

  cmn = frame->prefill - sizeof (sctp_cmn_hdr_t);
  ip = (u0 *) cmn - sizeof (ipv4_t);
  chk = ip->check;
  ip->len = htons (frame->plen - sizeof (mac_t));
  chk += htons (0) - ip->len;
  if (chk > 0xFFFF)
    chk = (chk & 0xFFFF) + (chk >> 16);
  ip->check = chk;

  cmn->check = ~(u32) generate_crc32c_on_crc32c ((const u8 *) cmn, sizeof (sctp_cmn_hdr_t ) + sizeof (sctp_fld_hdr_t) + sizeof (sctp_init_hdr_t), 0xFFFFFFFF);

  frame->prefill = packet;
  return frame;
}
