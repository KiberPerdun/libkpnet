//
// Created by KiberPerdun on 9/26/25.
//

#include "if_packet.h"

u0
eth_send_sctp (sctp_association_t *assoc)
{
  sctp_cmn_hdr_t *cmn;
  ipv4_t *ip;
  u32 chk;

  ip = assoc->current_packet + sizeof (mac_t);
  cmn = assoc->current_packet + sizeof (mac_t) + sizeof (ipv4_t);
  ip->len = htons ( assoc->mtu - assoc->remain_plen + sizeof (sctp_cmn_hdr_t) + sizeof (ipv4_t));
  chk = ip->check;
  chk += htons (0) - ip->len;
  if (chk > 0xFFFF)
    chk = (chk & 0xFFFF) + (chk >> 16);

  ip->check = chk;
  cmn->check = 0;
  cmn->check = ~(u32)generate_crc32c_on_crc32c (
      (const u8 *)cmn,
      assoc->mtu - assoc->remain_plen + sizeof (sctp_cmn_hdr_t),
      0xFFFFFFFF);

retry:
  if (eth_send (
          assoc->eth, assoc->current_packet,
          assoc->mtu - assoc->remain_plen
              + (sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t)))
      < 0)
    goto retry;

  else
    push_ringbuf (assoc->prefilled_ring, assoc->current_packet, 2048);
}
