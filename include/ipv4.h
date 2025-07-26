//
// Created by KiberPerdun on 9/25/24.
//

#ifndef LIBKPNET_IPV4_H
#define LIBKPNET_IPV4_H

#include "if_packet.h"

#define PROTO_SCTP 132

#define ip_check_carry(x)                                                     \
  (x = (x >> 16) + (x & 0xffff), (~(x + (x >> 16)) & 0xffff))

bool fill_ipv4(u32 src, u32 dst, u8 proto, u0 *args);
int ip_check_add (const void *buf, size_t len, int check);
u16 in_check (u16 *ptr, int nbytes);
/* frame_data_t *build_ip_raw (frame_data_t *frame, u32 src, u32 dst, u8 proto, u16 plen); */
u0 *build_ip_raw (u0 *packet, u16 *plen, u32 src, u32 dst, u8 proto);

#endif // LIBKPNET_IPV4_H
