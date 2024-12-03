//
// Created by KiberPerdun on 03.12.2024.
//

#include "tcp.h"
#include "if_packet.h"

bool
build_tcp_payload_hdr (u0 *_args, u0 *payload, u16 payload_len)
{
  connection_args_t *args;
  u0 *hdr;

  args = _args;

  if (!(hdr
        = build_tcp_raw (ntohl (args->tp_layer.tcp->seq), ntohl (args->tp_layer.tcp->ack), 0x18, 64240, 0, 0, NULL, args)))
    return false;

  pseudo_t psh;
  psh.source_address = args->net_layer.ipv4->src_addr;
  psh.dest_address = args->net_layer.ipv4->dest_addr;
  psh.placeholder = 0;
  psh.protocol = IPPROTO_TCP;

  psh.tcp_length = htons (sizeof (tcp_t) + payload_len);

  u8 *buffer = malloc (sizeof (pseudo_t) + sizeof (tcp_t) + payload_len);
  memcpy (buffer, &psh, sizeof (pseudo_t));
  memcpy (buffer + sizeof (pseudo_t), hdr, sizeof (tcp_t));
  memcpy (buffer + sizeof (pseudo_t) + sizeof (tcp_t), payload, payload_len);

  memcpy (args->tp_layer.tcp, hdr, sizeof (tcp_t));
  args->plen += sizeof (tcp_t);
  memcpy ((u0 *) args->tp_layer.tcp + sizeof (tcp_t), payload, payload_len);
  args->plen += payload_len;

  args->tp_layer.tcp->check = tcp_checksum ((u16 *) buffer, sizeof (pseudo_t) + sizeof (tcp_t) + payload_len);
  args->net_layer.ipv4->len = htons (sizeof (ipv4_t) + sizeof (tcp_t) + payload_len);

  args->net_layer.ipv4->checksum = 0;
  args->net_layer.ipv4->checksum = in_check ((u16 *) (args->net_layer.ipv4), sizeof (ipv4_t));

  free (buffer);
  free (hdr);

  return true;
}
