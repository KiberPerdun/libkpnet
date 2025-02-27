//
// Created by KiberPerdun on 2/14/25.
//

#include "checks.h"

frame_data_t *
fix_check_ip_tcp (frame_data_t *frame, u16 frm_max_len)
{
  pseudo_t psh;
  u0 *packet;
  ipv4_t *ip;
  tcp_t *tcp;

  if (NULL == frame->packet)
    return frame;

  packet = frame->packet - (frm_max_len - frame->plen) + sizeof (mac_t);
  tcp = packet + sizeof (ipv4_t);
  ip = packet;


  if (frame->sync)
    {
      u32 chk = ((frame_sync_ip_tcp_t *)frame->sync)->ip_check_part + ip->len;
      if (chk > 0xFFFF)
        chk = (chk & 0xFFFF) + (chk >> 16);

      ip->check = ~(u16)chk;
    }
  else
    {
      ip->check = 0;
      ip->check = ~(u16)ip_checksum ((u16 *)ip, sizeof (ipv4_t));
    }

  psh.source_address = ip->src_addr;
  psh.dest_address = ip->dest_addr;
  psh.placeholder = 0;
  psh.protocol = IPPROTO_TCP;
  psh.tcp_length = htons (sizeof (tcp_t));

  u8 *buffer
      = malloc (sizeof (pseudo_t) + sizeof (tcp_t));
  memcpy (buffer, &psh, sizeof (pseudo_t));
  memcpy (buffer + sizeof (pseudo_t), tcp, (ntohs (tcp->flags) >> 12 & 0XF) * 4);

  tcp->check = tcp_checksum ((u16 *) buffer, sizeof (pseudo_t) + sizeof (tcp_t));

  free (buffer);

  return frame;
}
