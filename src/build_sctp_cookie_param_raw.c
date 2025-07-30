//
// Created by KiberPerdun on 7/30/25.
//

#include "if_packet.h"

u0 *
build_sctp_cookie_param_raw (u0 *packet, u16 *plen, cookie_t *cookie)
{
  sctp_cookie_t *hdr;
  cookie_t *ck;

  if (NULL == packet || NULL == plen || NULL == cookie)
    return NULL;

  packet -= sizeof (sctp_cookie_t) + sizeof (cookie_t) + sizeof (u32);
  ck = packet + sizeof (sctp_cookie_t);
  hdr = packet;
  hdr->type = htons (7);
  hdr->len = htons (sizeof (sctp_cookie_t) + sizeof (cookie_t) + sizeof (u32));
  memcpy (ck, cookie, sizeof (cookie_t));
  /* в будушем надо MAC реализовать */
  u32 crc = generate_crc32c ((u8 *) cookie, sizeof (cookie_t));
  memcpy ((u0 *)ck + sizeof (cookie_t), &crc, sizeof (u32));

  *plen += ntohs (hdr->len);

  return packet;
}
