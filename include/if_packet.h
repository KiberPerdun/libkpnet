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

typedef struct packet_args
{
  u32 srcport;
  u32 dstport;
  u16 plen;
  u8 proto;

  union
  {
    ipv4_t ipv4;
    ipv6_t ipv6;
  } net_layer;

  union
  {
    struct
    {
      tcp_t _tcp;
      enum
      {
        TCP_SYN = 1,
        TCP_SYNACK = 2,
        TCP_ESTABLISHED = 3,
        TCP_TORN = 4,
      } TCP_STATUS;
    } tcp;

    udp_t udp;
  } tp_layer;
} packet_args_t;

typedef bool (*lrcall_t)(u0 *, u64, packet_args_t *);

u0 recv_filtered (i32 fd, lrcall_t filter, packet_args_t * args);
bool if_ipv4 (u0 *packet, u64 size, packet_args_t *args);
bool if_tcp (u0 *packet, u64 size, packet_args_t *args);
bool if_ipv4_tcp (u0 *packet, u64 size, packet_args_t *args);

#endif // LIBKPNET_IF_PACKET_H
