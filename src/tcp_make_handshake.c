//
// Created by KiberPerdun on 15.11.24.
//

#include "tcp.h"
#include "if_packet.h"

u0
tcp_make_handshake (u0 *ars, eth_t *e)
{
  packet_args_t *args;
  u0 *phase, *packet;

  args = ars;
  packet = args->packet + args->plen;

  phase = build_tcp_init_hdr (args->srcport, args->dstport, &args->plen, -1, &args->net_layer.ipv4);
  memcpy (packet, phase, sizeof (tcp_t) + sizeof (tcp_opt_mss_t));

  ((ipv4_t *)(args->packet + 14))->len = htons ( args->plen - 14);
  ((ipv4_t *)(args->packet + 14))->checksum = 0;
  ((ipv4_t *)(args->packet + 14))->checksum = in_check ((u16 *) (args->packet + 14), sizeof (ipv4_t));

  args->tp_layer.tcp._tcp = *((tcp_t *) phase);
  free (phase);
  args->tp_layer.tcp.TCP_STATUS = 1;

  eth_send (e, args->packet, args->plen);

  recv_filtered (e->fd, if_ipv4_tcp, args);

  if (args->tp_layer.tcp.TCP_STATUS != TCP_SYNACK)
    {
      args->tp_layer.tcp.TCP_STATUS = TCP_TORN;
      return;
    }

  args->plen -= 24;

  phase = build_tcp_ack_hdr (args->srcport, args->dstport, &args->plen, args);
  ((ipv4_t *)(args->packet + 14))->len = htons ( args->plen - 14);
  ((ipv4_t *)(args->packet + 14))->checksum = 0;
  ((ipv4_t *)(args->packet + 14))->checksum = in_check ((u16 *) (args->packet + 14), sizeof (ipv4_t));
  memcpy (packet, phase, sizeof (tcp_t));

  eth_send (e, args->packet, args->plen);
  args->tp_layer.tcp.TCP_STATUS = TCP_ESTABLISHED;

  args->plen -= sizeof (tcp_t);

  free (phase);
}
