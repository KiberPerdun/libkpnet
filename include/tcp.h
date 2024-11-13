//
// Created by KiberPerdun on 22.10.24.
//

#ifndef LIBKPNET_TCP_H
#define LIBKPNET_TCP_H

#include "eth.h"
#include "ipv4.h"
#include "types.h"
#include <assert.h>

typedef struct tcp_hdr {
  u16 srcp;
  u16 dstp;
  u32 seq;
  u32 ack;
  u16 flags;
  u16 win;
  u16 check;
  u16 urgent;
} tcp_t;

typedef struct tcp_option {
  u8 type;
  u8 len;
} tcp_opt_t;

typedef struct tcp_option_mss {
  tcp_opt_t opt;
  u16 mss;
} tcp_opt_mss_t;

tcp_t *tcp_fill_init_hdr (u16 src, u16 dst, u16 flags);
tcp_opt_mss_t *tcp_fill_opt_mss (u16 mss);
u16 tcp_checksum (u16 *ptr, int nbytes);
u0 *build_tcp_init_hdr (u16 src_port, u16 dst_port, u16 *plen, u16 mss,
                        ipv4_t *ip);
u0 *build_tcp_raw (u16 src_port, u16 dst_port, u32 seq, u32 ack, u16 flags,
                   u16 win, u16 urgent, u8 optlen);

#endif // LIBKPNET_TCP_H
