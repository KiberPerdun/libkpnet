//
// Created by KiberPerdun on 3/27/25.
//

#include "checks.h"
#include "if_packet.h"

u0 *
if_ip_sctp (u0 *packet, u16 size, u0 *meta)
{
  if (NULL == meta || NULL == packet)
    return NULL;

  if (size <= sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t) + sizeof (sctp_fld_hdr_t))
    return NULL;

  ipv4_t *ip = packet + sizeof (mac_t);
  sctp_t *sctp = packet + sizeof (mac_t) + sizeof (ipv4_t);
  if_ip_sctp_meta_t *m = meta;
  if (ip->dest_addr != m->src_ip)
    return NULL;

  u16 check_check_ip = ip->check;
  ip->check = 0;
  if (check_check_ip != (u16)~(u16)ip_checksum ((u16 *)ip, sizeof (ipv4_t)))
    return NULL;

  u32 check_check_sctp = sctp->cmn.check;
  sctp->cmn.check = 0;
  if (check_check_sctp != generate_crc32c ((const u8 *)sctp, sizeof (sctp_cmn_hdr_t) + ntohs (sctp->fld.len)))
    return NULL;

  if (m->src_port != ntohs (sctp->cmn.dstp))
    return NULL;

  puts ("SCTP packet received");

  switch (sctp->fld.type)
    {
    case SCTP_INIT:
      {
        if (m->state == SCTP_LISTEN)
          {
            m->dst_port = ntohs (sctp->cmn.srcp);
            m->state = SCTP_INIT_RECEIVED;

            m->ver_tag = sctp->type.init.init_tag;
            m->a_rwnd = ntohs (sctp->type.init.a_rwnd);
            m->os = ntohs (sctp->type.init.os);
            m->mis = ntohs (sctp->type.init.mis);

            m->dst_ip = ip->src_addr;
            break;
          }
        return NULL;
      }
    case SCTP_INIT_ACK:
      {
        if (m->state == SCTP_INIT_SENT)
          {
            m->dst_port = ntohs (sctp->cmn.srcp);
            m->state = SCTP_INIT_ACK_RECEIVED;

            m->init_tag = sctp->type.init_ack.init_tag;
            m->a_rwnd = sctp->type.init_ack.a_rwnd;
            m->os = ntohs (sctp->type.init_ack.os);
            m->mis = ntohs (sctp->type.init_ack.mis);
            m->tsn = sctp->type.init_ack.init_tsn;

            if (ntohs (sctp->type.cookie.type) != 7)
              return NULL;

            m->add_len = ntohs (sctp->type.cookie.len) - sizeof (sctp_cookie_t);
            if (NULL == (m->add = malloc (m->add_len)))
              return NULL;

            memcpy (m->add, (u0 *) sctp + sizeof (sctp_cmn_hdr_t) + sizeof (sctp_fld_hdr_t) + sizeof (sctp_init_ack_hdr_t) + sizeof (sctp_cookie_t), m->add_len);

            break;
          }
        return NULL;
      }
    case SCTP_COOKIE_ECHO:
      {
        if (m->state == SCTP_LISTEN || m->state == SCTP_INIT_RECEIVED)
          {
            sctp_fld_hdr_t *cookie_echo = &sctp->fld;
            if (ntohs (cookie_echo->len) < sizeof (cookie_t) + sizeof (sctp_cookie_t) + HMAC_MD5_KEY_LEN)
              return NULL;

            if ((m->add = calloc (sizeof (if_ip_sctp_meta_t), 1)) == NULL)
              return NULL;

            memcpy (m->add, cookie_echo + 1, sizeof (cookie_t));

            u8 key[16] = {
              0xC9, 0xAB, 0x02, 0x9C, 0x92, 0xBF, 0x47, 0xF3,
              0xC9, 0xAB, 0x02, 0x9C, 0x92, 0xBF, 0x47, 0xF3
            };

            u8 digest[16];
            u8 res;

            hmac_md5 ((u8 *) (cookie_echo + 1), sizeof (cookie_t), key, 16, digest);

            res = 0;
            for (int i = 0; i < HMAC_MD5_KEY_LEN; ++i)
                res |= digest[i] ^ *((u8 *) (cookie_echo + 1) + sizeof(cookie_t) + i);

            if (res != 0)
              printf ("HMAC INVALID");

            break;
          }
        return NULL;
      }
    default:
      return NULL;
    }

  return meta;
}
