//
// Created by KiberPerdun on 3/27/25.
//

#include "if_packet.h"

u0 *
if_ip_sctp_init (u0 *packet, u16 size, u0 *meta)
{
  if (NULL == meta || NULL == packet)
    return NULL;

  if (size >= sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t))
    return NULL;

  ipv4_t *ip = packet + sizeof (mac_t);
  if_ip_sctp_meta_t *m = meta;
  if (!(ip->src_addr == m->dst_ip && ip->dest_addr == m->src_ip))
    return NULL;


  return meta;
}
