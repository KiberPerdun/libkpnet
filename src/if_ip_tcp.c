//
// Created by KiberPerdun on 6/6/25.
//

#include "if_packet.h"
#include "checks.h"

u0 *
if_ip_tcp (u0 *packet, u16 size, u0 *meta)
{
  if (NULL == meta || NULL == packet)
    return NULL;

  if (size <= sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t) + sizeof (sctp_fld_hdr_t))
    return NULL;

  ipv4_t *ip = packet + sizeof (mac_t);
  tcp_t *tcp = packet + sizeof (mac_t) + sizeof (ipv4_t);
  if_ip_tcp_meta_t *m = meta;
  if (m->src_ip != ip->dest_addr)
    return NULL;

  u16 check_check_ip = ip->check;
  ip->check = 0;
  if (check_check_ip != (u16)~(u16)ip_checksum ((u16 *)ip, sizeof (ipv4_t)))
    return NULL;

  u16 check_tmp = tcp->check;
  tcp->check = 0;

  pseudo_t psh;

  psh.source_address = ip->src_addr;
  psh.dest_address = ip->dest_addr;
  psh.placeholder = 0;
  psh.protocol = IPPROTO_TCP;
  psh.tcp_length = htons (sizeof (tcp_t));

  u8 offset;
  offset = (ntohs (tcp->flags) >> 12 & 0XF) * 4;

  u8 *buffer = malloc (sizeof (pseudo_t) + offset);
  memcpy (buffer, &psh, sizeof (pseudo_t));
  memcpy (buffer + sizeof (pseudo_t), tcp, offset);

  tcp->check = tcp_checksum ((u16 *) buffer, sizeof (pseudo_t) + offset);

  if (check_tmp != tcp->check)
    return NULL;

  if ((ntohs (tcp->flags) & 0x1FF) == 0x2)
    {
      puts ("syn received");
      m->dst_ip = ip->src_addr;
      m->dst_port = tcp->srcp;
      m->dst_seq = tcp->seq;
    }
  else if ((ntohs (tcp->flags) & 0x1FF) == 0x12)
    m->dst_seq = tcp->seq;

  free (buffer);

  return meta;
}
