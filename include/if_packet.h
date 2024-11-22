//
// Created by KiberPerdun on 14.11.24.
//

#ifndef LIBKPNET_IF_PACKET_H
#define LIBKPNET_IF_PACKET_H

#include "if_packet.h"
#include "ipv4.h"
#include "ipv6.h"
#include "stdbool.h"
#include "tcp.h"
#include "types.h"
#include "udp.h"

#define MAX_PACKET_LEN 1516

typedef struct connection_args
{
  u0 *packet;
  u16 plen;
  u8 proto;
  u32 srcport;
  u32 dstport;
  eth_t *eth;

  union
  {
    ipv4_t *ipv4;
    ipv6_t *ipv6;
  } net_layer;

  union
  {
    union
    {
      tcp_t *_tcp;
      enum
      {
        TCP_LISTEN = 1,
        TCP_SYN_SENT = 2,
        TCP_SYN_RECEIVED = 3,
        TCP_ESTABLISHED_CONNECTON = 4,
        TCP_FIN_WAIT_1 = 5,
        TCP_FIN_WAIT_2 = 6,
        TCP_CLOSE_WAIT = 7,
        TCP_CLOSING = 8,
        TCP_LAST_ACK = 9,
        TCP_TIME_WAIT = 10,
        TCP_CLOSED = 11,
      } TCP_STATUS;
    } tcp;

    udp_t *udp;
  } tp_layer;
} connection_args_t;

typedef bool (*lrcall_t)(u0 *, u64, connection_args_t *);

u0 recv_filtered (i32 fd, lrcall_t filter, connection_args_t * args);
bool if_ipv4 (u0 *packet, u64 size, connection_args_t *args);
bool if_tcp (u0 *packet, u64 size, connection_args_t *args);
bool if_ipv4_tcp (u0 *packet, u64 size, connection_args_t *args);

#endif // LIBKPNET_IF_PACKET_H
