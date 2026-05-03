//
// Created by KiberPerdun on 10.01.26.
//

#include "if_packet.h"
#include "random.h"

i64
prefill_sctp_mac_ip (sctp_association_t *assoc, u64 mac_gateway, u64 mac_dev)
{
  u0 *packet;

  KP_ALIGN_CACHE struct
  {
    u8 offset[NET_IP_ALIGN];
    mac_t eth;
    ipv4_t ip;
    sctp_cmn_hdr_t sctp;
  } __attribute__ ((packed)) hdr;

  memset (&hdr, 0, sizeof (hdr));

  packet = assoc->prefilled_umem_packet;

  hdr.sctp.srcp = __builtin_bswap16 (assoc->ulp->src_port);
  hdr.sctp.dstp = __builtin_bswap16 (assoc->ulp->dst_port);

  hdr.ip.ihl = 5;
  hdr.ip.ver = 4;
  hdr.ip.tos = 0x2;
  hdr.ip.indent = get_random_from_buffer_u32 ();
  hdr.ip.ttl = 141;
  hdr.ip.proto = PROTO_SCTP;
  hdr.ip.src_addr = assoc->ulp->src_ip;
  hdr.ip.dest_addr = assoc->ulp->dst_ip;

  memcpy (hdr.eth.mac, &mac_gateway, 6);
  memcpy (hdr.eth.mac + 6, &mac_dev, 6);
  hdr.eth.type = __builtin_bswap16 (ETHERTYPE_IP);
  memcpy (packet, (u8 *) &hdr, sizeof (hdr));
  assoc->prefilled_umem_packet_len = sizeof (hdr) - NET_IP_ALIGN;
  assoc->prefilled_umem_packet += NET_IP_ALIGN;

  return 0;
}
