//
// Created by KiberPerdun on 7/30/25.
//

#include "if_packet.h"

u0 *
build_sctp_cookie_echo_hdr_raw (u0 *packet, u16 *plen, u0 *cookie, u16 cookie_len)
{
  if (NULL == packet || NULL == plen)
    return NULL;

  packet -= cookie_len;
  memcpy (packet, cookie, cookie_len);
  *plen += cookie_len;

  return packet;
}
