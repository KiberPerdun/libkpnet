//
// Created by KiberPerdun on 9/25/25.
//

#include "if_packet.h"
#include "prefilled.h"

/*
 * packet: ip
 */
i64
sctp_process_sctp_init (sctp_association_t *assoc, u0 *packet, u32 plen)
{
  sctp_association_t ass;
  sctp_t *sctp;
  ipv4_t *ip;

  ip = packet;
  sctp = (u0 *) ip + sizeof (ipv4_t);

  ass.dst_port = sctp->cmn.srcp;
  ass.dst_ip = ip->src_addr;
  ass.os = sctp->type.init.os;
  ass.mis = sctp->type.init.mis;
  ass.a_rwnd = sctp->type.init.a_rwnd;
  ass.ver_tag = sctp->type.init.init_tag;
  ass.ulp = assoc->ulp;
  ass.tx_ring = assoc->tx_ring;
  ass.prefilled_ring = assoc->prefilled_ring;
  ass.tsn = sctp->type.init.init_tsn;

  build_prefilled_mac_ip_sctp_init_ack_hdr (&ass);

  return 0;
}
