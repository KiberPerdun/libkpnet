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

  packet -= sizeof (sctp_cookie_t) + sizeof (cookie_t) + HMAC_MD5_KEY_LEN;
  ck = packet + sizeof (sctp_cookie_t);
  hdr = packet;
  hdr->type = htons (7);
  hdr->len = htons (sizeof (sctp_cookie_t) + sizeof (cookie_t) + HMAC_MD5_KEY_LEN);
  memcpy (ck, cookie, sizeof (cookie_t));

  u8 key[16] = {
    0xC9, 0xAB, 0x02, 0x9C, 0x92, 0xBF, 0x47, 0xF3,
    0xC9, 0xAB, 0x02, 0x9C, 0x92, 0xBF, 0x47, 0xF3
  };

  hmac_md5 ((u8 *) cookie, sizeof (cookie_t), key, HMAC_MD5_KEY_LEN,(u0 *)ck + sizeof (cookie_t));

  *plen += ntohs (hdr->len);

  return packet;
}
