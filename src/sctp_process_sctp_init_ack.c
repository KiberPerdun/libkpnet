//
// Created by KiberPerdun on 10/2/25.
//

#include "if_packet.h"
#include "prefilled.h"

i64
sctp_process_sctp_init_ack (sctp_association_t *assoc, u0 *packet, u32 plen)
{
  struct
  {
    sctp_init_ack_hdr_t init_ack;
    sctp_cookie_t cookie;
  } *hdr;

  if (ntohs (plen) <= sizeof (sctp_fld_hdr_t) + sizeof (sctp_init_ack_hdr_t) + sizeof (sctp_cookie_t))
    return -1;

  plen = ntohs (plen) - (sizeof (sctp_fld_hdr_t) + sizeof (sctp_init_ack_hdr_t) + sizeof (sctp_cookie_t));
  hdr = packet;

  pthread_spin_lock (&assoc->lock);
  assoc->ver_tag = hdr->init_ack.init_tag;
  assoc->os = hdr->init_ack.os;
  assoc->mis = hdr->init_ack.mis;
  assoc->a_rwnd = hdr->init_ack.a_rwnd;
  assoc->ver_tag = hdr->init_ack.init_tag;
  assoc->tsn = hdr->init_ack.init_tsn;
  pthread_spin_unlock (&assoc->lock);

  /* only for sctp_cookie_echo */
  assoc->id = plen;
  assoc->retry_ring = (u0 *) (hdr + 1);

  build_prefilled_mac_ip_sctp_cookie_echo_hdr (assoc);

  return 0;
}
