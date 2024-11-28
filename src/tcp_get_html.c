//
// Created by KiberPerdun on 16.11.2024.
//

#include "tcp.h"
#include "if_packet.h"

u0 *
tcp_get_html (u0 *ars)
{
  tcp_t *phase;
  connection_args_t *args;
  u0 *buf, *packet;
  u16 payload_len;

  /*

  const char *str = "GET / HTTP/1.0\r\n\r\n";

  args = ars;
  packet = args->packet;
  if (!(buf = calloc (1, args->tp_layer.tcp._tcp.win)))
    return NULL;

  phase = build_tcp_raw (args->srcport, args->dstport, ntohl (args->tp_layer.tcp._tcp.seq), ntohl (args->tp_layer.tcp._tcp.ack), 0x10, 64240, 0, 0, NULL, NULL);
  payload_len = strlen (str);
  memcpy (packet + 34 + sizeof (tcp_t), str, payload_len);

  typedef struct pseudo_header
  {
    u32 source_address;
    u32 dest_address;
    u8 placeholder;
    u8 protocol;
    u16 tcp_length;
  } pseudo_t;

  pseudo_t psh;
  psh.source_address = args->net_layer.ipv4.src_addr;
  psh.dest_address = args->net_layer.ipv4.dest_addr;
  psh.placeholder = 0;
  psh.protocol = IPPROTO_TCP;

  psh.tcp_length = htons (sizeof (tcp_t) + payload_len);

  u8 *buffer
      = calloc (1, sizeof (pseudo_t) + sizeof (tcp_t) + payload_len);
  memcpy (buffer, &psh, sizeof (pseudo_t));
  memcpy (buffer + sizeof (pseudo_t), phase,
          sizeof (tcp_t));
  memcpy (buffer + sizeof (pseudo_t) + sizeof (tcp_t), str, payload_len);

  ((tcp_t *)phase)->check
      = tcp_checksum ((u16 *)buffer, sizeof (struct pseudo_header) + sizeof (tcp_t) + payload_len);
  memcpy (packet + 34, phase, sizeof (tcp_t));
  free (buffer);
  free (phase);

  ((ipv4_t *)(args->packet + 14))->len = htons ( 54 + payload_len - 14);
  ((ipv4_t *)(args->packet + 14))->checksum = 0;
  ((ipv4_t *)(args->packet + 14))->checksum = in_check ((u16 *) (args->packet + 14), sizeof (ipv4_t));
  eth_send (e, packet, 54 + payload_len);

  recv_filtered (e->fd, if_ipv4_tcp, args);

  puts (args->data);
  free (args->data);

   */

  return buf;
}

