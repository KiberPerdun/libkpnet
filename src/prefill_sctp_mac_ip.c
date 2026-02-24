//
// Created by KiberPerdun on 10.01.26.
//

#include "if_packet.h"
#include "random.h"

i64
prefill_sctp_mac_ip (sctp_association_t *assoc, u64 mac_gateway, u64 mac_dev)
{
  sctp_cmn_hdr_t cmn;
  u0 *packet;

  struct
  {
    ipv4_t ip;
    sctp_cmn_hdr_t sctp;
  } hdr;

  packet = assoc->prefilled_umem_packet;

  hdr.sctp.check = 0;
  hdr.sctp.srcp = htons (assoc->ulp->src_port);
  hdr.sctp.dstp = htons (assoc->ulp->dst_port);
  hdr.sctp.tag = 0;

  hdr.ip.ihl = 5;
  hdr.ip.ver = 4;
  hdr.ip.tos = htons (0x2);
  hdr.ip.indent = get_random_from_buffer_u32 ();
  hdr.ip.offset = 0;
  hdr.ip.ttl = 141;
  hdr.ip.proto = PROTO_SCTP;
  hdr.ip.src_addr = assoc->ulp->src_ip;
  hdr.ip.dest_addr = assoc->ulp->dst_ip;
  hdr.ip.check = 0;
  hdr.ip.len = htons (0);
  hdr.ip.check = 0;

  memcpy (packet, &mac_gateway, 6);
  memcpy (packet + 6, &mac_dev, 6);
  u16 type_be = htons (ETHERTYPE_IP);
  memcpy (packet + 12, &type_be, 2);
  memcpy (packet + 14, &hdr, sizeof (hdr));
  assoc->prefilled_umem_packet_len = sizeof (hdr) + 14;
 // assoc->umem_hdrs_basic_len = sizeof (hdr) + 14;
  //assoc->umem_hdrs_cursor = assoc->umem_hdrs + assoc->umem_hdrs_basic_len;

  return 0;
}
