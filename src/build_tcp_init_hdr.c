//
// Created by KiberPerdun on 25.10.24.
//
#include "tcp.h"

u0 *
build_tcp_init_hdr (u16 src_port, u16 dst_port, u16 *plen, u16 mss_size, ipv4_t *ip)
{
  u0 *hdr;

  assert (plen);
  assert (ip);
  assert (*plen >= 0);

  if (!(hdr
        = build_tcp_raw (src_port, dst_port, 2856147168, 0, 0x2, 64240, 0, 4)))
    return NULL;

  *plen += sizeof (tcp_t) + sizeof (tcp_opt_mss_t);
  tcp_opt_mss_t *mss = tcp_fill_opt_mss (mss_size);
  memcpy (hdr + sizeof (tcp_t), mss, sizeof (tcp_opt_mss_t));

  typedef struct pseudo_header
  {
    u32 source_address;
    u32 dest_address;
    u8 placeholder;
    u8 protocol;
    u16 tcp_length;
  } pseudo_t;

  pseudo_t psh;
  psh.source_address = ip->src_addr;
  psh.dest_address = ip->dest_addr;
  psh.placeholder = 0;
  psh.protocol = IPPROTO_TCP;

  psh.tcp_length = htons (sizeof (tcp_t) + sizeof (tcp_opt_mss_t));

  u8 *buffer
      = malloc (sizeof (pseudo_t) + sizeof (tcp_t) + sizeof (tcp_opt_mss_t));
  memcpy (buffer, &psh, sizeof (pseudo_t));
  memcpy (buffer + sizeof (pseudo_t), hdr,
          sizeof (tcp_t) + sizeof (tcp_opt_mss_t));

  ((tcp_t *)hdr)->check
      = tcp_checksum ((u16 *)buffer, sizeof (struct pseudo_header) + 24);
  free (buffer);
  free (mss);

  return hdr;
}
