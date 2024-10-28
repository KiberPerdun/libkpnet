//
// Created by KiberPerdun on 25.10.24.
//
#include "tcp.h"

void *
build_tcp_init_hdr (u16 src_port, u16 dst_port, u16 *plen, u16 mss_size, ipv4_t *ip)
{
  void *packet;

  packet = malloc (sizeof (tcp_t) + sizeof (tcp_opt_mss_t));
  *plen += sizeof (tcp_t) + sizeof (tcp_opt_mss_t);
  tcp_t *tcp = tcp_fill_init_hdr (src_port, dst_port, 0x2);
  memcpy (packet, tcp, sizeof (tcp_t));
  tcp_opt_mss_t *mss = tcp_fill_opt_mss (mss_size);
  memcpy (packet + sizeof (tcp_t), mss, sizeof (tcp_opt_mss_t));

  struct pseudo_header {
    u32 source_address;
    u32 dest_address;
    u8 placeholder;
    u8 protocol;
    u16 tcp_length;
  };

  struct pseudo_header psh;
  psh.source_address = ip->src_addr;
  psh.dest_address = ip->dest_addr;
  psh.placeholder = 0;
  psh.protocol = IPPROTO_TCP;

  psh.tcp_length = htons (sizeof (tcp_t) + sizeof (tcp_opt_mss_t));

  u8 *buffer = malloc (sizeof (struct pseudo_header) + 24);
  memcpy (buffer, &psh, sizeof (struct pseudo_header));
  memcpy (buffer + sizeof (struct pseudo_header), packet, sizeof (tcp_t) + sizeof (tcp_opt_mss_t));

  ((tcp_t *) packet)->check = tcp_checksum ((u16 *) buffer, sizeof (struct pseudo_header) + 24);
  free (buffer);
  free (mss);

  return packet;
}
