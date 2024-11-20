//
// Created by KiberPerdun on 9/25/24.
//

#ifndef LIBKPNET_IPV4_H
#define LIBKPNET_IPV4_H

#include "endian.h"
#include "types.h"
#include "eth.h"

#define PROTO_SCTP 132

typedef struct ipv4_hdr {
#if __BYTE_ORDER__ == __LITTLE_ENDIAN
  u8  ihl:4;
  u8  ver:4;
#endif
#if __BYTE_ORDER__ == __BIG_ENDIAN__
  u8  ver:4;
  u8  ihl:4;
#endif

  u8  tos;
  u16 len;

  u16 indent;
  u16 offset;

  u8  ttl;
  u8  proto;
  u16 checksum;
  u32 src_addr;
  u32 dest_addr;
} ipv4_t;

#define ip_check_carry(x)                                                     \
  (x = (x >> 16) + (x & 0xffff), (~(x + (x >> 16)) & 0xffff))

ipv4_t *fill_ipv4(u32 src, u32 dst, u8 proto);
int ip_check_add (const void *buf, size_t len, int check);
u16 in_check (u16 *ptr, int nbytes);

#endif // LIBKPNET_IPV4_H
