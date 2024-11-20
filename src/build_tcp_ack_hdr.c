//
// Created by KiberPerdun on 14.11.24.
//

#include "tcp.h"
#include "if_packet.h"

u0 *
build_tcp_ack_hdr (u16 src_port, u16 dst_port, u16 *plen, u0 *args)
{
  packet_args_t *arg;
  ipv4_t ip;
  u0 *hdr;

  assert (plen);
  assert (args);
  assert (*plen >= 0);

  arg = args;
  ip = arg->net_layer.ipv4;

  if (!(hdr
        = build_tcp_raw (src_port, dst_port, ntohl (arg->tp_layer.tcp._tcp.seq), ntohl (arg->tp_layer.tcp._tcp.ack += htonl (1)), 0x10, 64240, 0, 0)))
    return NULL;

  *plen += sizeof (tcp_t);

  typedef struct pseudo_header
  {
    u32 source_address;
    u32 dest_address;
    u8 placeholder;
    u8 protocol;
    u16 tcp_length;
  } pseudo_t;

  pseudo_t psh;
  psh.source_address = ip.src_addr;
  psh.dest_address = ip.dest_addr;
  psh.placeholder = 0;
  psh.protocol = IPPROTO_TCP;

  psh.tcp_length = htons (sizeof (tcp_t));

  u8 *buffer
      = malloc (sizeof (pseudo_t) + sizeof (tcp_t));
  memcpy (buffer, &psh, sizeof (pseudo_t));
  memcpy (buffer + sizeof (pseudo_t), hdr,
          sizeof (tcp_t));

  ((tcp_t *)hdr)->check
      = tcp_checksum ((u16 *)buffer, sizeof (struct pseudo_header) + sizeof (tcp_t));
  free (buffer);

  return hdr;
}
