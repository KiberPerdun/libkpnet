//
// Created by KiberPerdun on 22.10.24.
//

#ifndef LIBKPNET_TCP_H
#define LIBKPNET_TCP_H

#include "eth.h"
#include "ipv4.h"
#include "types.h"
#include <assert.h>
#include <stdbool.h>

typedef struct pseudo_header
{
  u32 source_address;
  u32 dest_address;
  u8 placeholder;
  u8 protocol;
  u16 tcp_length;
} pseudo_t;

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

bool tcp_fill_init_hdr (u0 *args);
tcp_opt_mss_t *tcp_fill_opt_mss (u16 mss);
u16 tcp_checksum (u16 *ptr, int nbytes);
bool build_tcp_init_hdr (u0 *args);
bool build_tcp_ack_hdr (u0 *args);
u0 *build_tcp_raw (u32 seq, u32 ack, u16 flags,
                   u16 win, u16 urgent, u8 optlen, u0 *options, u0 *args);
u0 tcp_make_handshake (u0 *args);
u0 *tcp_get_html (u0 *args);

#endif // LIBKPNET_TCP_H
