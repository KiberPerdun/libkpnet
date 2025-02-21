//
// Created by KiberPerdun on 15.11.24.
//

#include "tcp.h"
#include "if_packet.h"

/*
u0
tcp_make_handshake (u0 *ars)
{
  connection_args_t *args;

  args = ars;

  build_tcp_init_hdr (args);
  eth_send (args->eth, args->packet, args->plen);

  args->TCP_STATUS = TCP_SYN_SENT;

  recv_filtered (args->eth->fd, if_ipv4_tcp, args);

  if (args->TCP_STATUS != TCP_SYN_RECEIVED)
    {
      args->TCP_STATUS = TCP_CLOSED;
      return;
    }

  build_tcp_ack_hdr (args);
  eth_send (args->eth, args->packet, args->plen);

  if (args->TCP_STATUS == TCP_SYN_RECEIVED)
    args->TCP_STATUS = TCP_ESTABLISHED_CONNECTON;

}
*/
