//
// Created by KiberPerdun on 10/6/24.
//

#ifndef LIBKPNET_IPV6_H
#define LIBKPNET_IPV6_H
#include "ipv4.h"

typedef struct ipv6_hdr
{
  u8 flags[4];
  u16 len;
  u8 proto;
  u8 ttl;
  u16 src;
  u16 dst;
} ipv6_t;

#endif // LIBKPNET_IPV6_H
