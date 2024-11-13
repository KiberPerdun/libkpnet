//
// Created by KiberPerdun on 10/6/24.
//

#ifndef LIBKPNET_UDP_H
#define LIBKPNET_UDP_H
#include "sctp.h"
#include "types.h"

typedef struct udp_hdr
{
  u16 srcp;
  u16 dstp;
  u16 len;
  u16 sum;
} udp_t;

udp_t *fill_udp (u16 src, u16 dst, u16 plen);

#endif // LIBKPNET_UDP_H
