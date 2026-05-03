//
// Created by KiberPerdun on 9/26/25.
//

#include "if_packet.h"

i64
eth_send_sctp (sctp_association_t *assoc)
{
  struct __attribute__ ((packed))
  {
    u8 mac[14];
    ipv4_t ip;
    sctp_cmn_hdr_t sctp;
  } *hdr;
  hdr = assoc->prefilled_umem_packet;
  hdr->ip.len = htons (assoc->mtu - assoc->remain_plen + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t));

retry:
  if (xdp_tx (assoc->xdp, assoc->xdp_segs, assoc->xdp_seg_lens, assoc->xdp_seg_count) <= 0)
    {
      _mm_pause ();
      goto retry;
    }

  /*
  else
    push_ringbuf (assoc->prefilled_ring, assoc->current_packet, 2048);
    */

  return 0;
}
