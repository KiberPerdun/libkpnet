//
// Created by KiberPerdun on 14.11.24.
//

#include "tcp.h"
#include "if_packet.h"

bool
build_tcp_ack_hdr (u0 *ars)
{
  connection_args_t *args;
  u0 *hdr;

  args = ars;

  if (!(hdr
        = build_tcp_raw (ntohl (args->tp_layer.tcp._tcp->seq), ntohl (args->tp_layer.tcp._tcp->ack += htonl (1)), 0x10, 64240, 0, 0, NULL, args)))
    return false;

  pseudo_t psh;
  psh.source_address = args->net_layer.ipv4->src_addr;
  psh.dest_address = args->net_layer.ipv4->dest_addr;
  psh.placeholder = 0;
  psh.protocol = IPPROTO_TCP;

  psh.tcp_length = htons (sizeof (tcp_t));

  u8 *buffer
      = malloc (sizeof (pseudo_t) + sizeof (tcp_t));
  memcpy (buffer, &psh, sizeof (pseudo_t));
  memcpy (buffer + sizeof (pseudo_t), hdr,
          sizeof (tcp_t));

  memcpy (args->tp_layer.tcp._tcp, hdr, sizeof (tcp_t));
  args->plen += sizeof (tcp_t);

  args->tp_layer.tcp._tcp->check = tcp_checksum ((u16 *) buffer, sizeof (pseudo_t) + sizeof (tcp_t));
  args->net_layer.ipv4->len = htons (sizeof (ipv4_t) + sizeof (tcp_t));

  args->net_layer.ipv4->checksum = 0;
  args->net_layer.ipv4->checksum = in_check ((u16 *) (args->net_layer.ipv4), sizeof (ipv4_t));

  free (buffer);
  free (hdr);

  return true;
}
