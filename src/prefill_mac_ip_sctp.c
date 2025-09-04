//
// Created by KiberPerdun on 9/4/25.
//

#include "checks.h"
#include "prefilled.h"
#include "random.h"

frame_data_t *prefill_mac_ip_sctp (frame_data_t *frame)
{
  if_ip_sctp_meta_t *meta;
  sctp_cmn_hdr_t cmn;
  u0 *packet;
  ipv4_t ip;

  if (__builtin_expect (frame == NULL, 0) ||  __builtin_expect (frame->prefill == NULL || frame->state == NULL, 0))
    return NULL;

  packet = frame->prefill;
  meta = frame->state;

  /* building sctp common header */
  cmn.check = 0;
  cmn.srcp = htons (meta->src_port);
  cmn.dstp = htons (meta->dst_port);
  cmn.tag = 0;

  /* building IPv4 header */
  ip.ihl = 5;
  ip.ver = 4;
  ip.tos = htons (0x2);
  ip.indent = get_random_u32 ();
  ip.offset = 0;
  ip.ttl = 228;
  ip.proto = PROTO_SCTP;
  ip.src_addr = meta->src_ip;
  ip.dest_addr = meta->dst_ip;
  ip.check = 0;
  ip.len = htons (0);
  ip.check = ~(u16) ip_checksum ((u16 *) &ip, sizeof (ipv4_t));

  memcpy (packet, meta->gateway, 6);
  memcpy ( packet + 6, meta->dev, 6);
  u16 type_be = htons (ETHERTYPE_IP);
  memcpy (packet + 12, &type_be, 2);

  memcpy (packet + (frame->plen += sizeof (mac_t)), &ip, sizeof (ipv4_t));
  memcpy (packet + (frame->plen += sizeof (ipv4_t)), &cmn, sizeof (sctp_cmn_hdr_t ));
  frame->plen += sizeof (sctp_cmn_hdr_t);

  return frame;
}
