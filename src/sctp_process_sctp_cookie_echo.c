//
// Created by KiberPerdun on 9/25/25.
//

#include "if_packet.h"
#include "prefilled.h"

i64
sctp_process_sctp_cookie_echo (sctp_association_t *assoc, u0 *packet, u32 plen)
{
  if (ntohs (plen) < sizeof (cookie_t) + sizeof (sctp_cookie_t) + HMAC_MD5_KEY_LEN)
    return -1;

  sctp_cookie_echo_hdr_t *hdr = packet;
  cookie_t *cookie = (cookie_t *) (hdr + 1);
  
  u8 key[16] = { 0xC9, 0xAB, 0x02, 0x9C, 0x92, 0xBF, 0x47, 0xF3,
                 0xC9, 0xAB, 0x02, 0x9C, 0x92, 0xBF, 0x47, 0xF3 };
  u8 digest[16];
  u8 res;

  hmac_md5 ((u8 *) (hdr + 1), sizeof (cookie_t), key, 16, digest);

  res = 0;
  for (i32 i = 0; i < HMAC_MD5_KEY_LEN; ++i)
    res |= digest[i] ^ *((u8 *) (hdr + 1) + sizeof (cookie_t) + i);

  if (res != 0)
    {
      printf ("HMAC INVALID");
      return -1;
    }

  pthread_spin_lock (&assoc->lock);
  assoc->ver_tag = cookie->ver_tag;
  assoc->init_tag = cookie->init_tag;
  assoc->a_rwnd = cookie->a_rwnd;
  assoc->os = cookie->os;
  assoc->mis = cookie->mis;
  assoc->tsn = cookie->tsn;
  assoc->dst_tsn = cookie->dst_tsn;
  assoc->dst_ip = cookie->src_ip;
  assoc->dst_port = cookie->src_port;

  assoc->rto = SCTP_RTO_MIN;
  assoc->srtt = 0;
  assoc->rttvar = 0;
  pthread_spin_unlock (&assoc->lock);

  build_prefilled_mac_ip_sctp_cookie_ack_hdr (assoc);

  prepare_sctp_threads (assoc);

  return 0;
}
