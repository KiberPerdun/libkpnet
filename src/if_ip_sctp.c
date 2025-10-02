//
// Created by KiberPerdun on 3/27/25.
//

#include "checks.h"
#include "if_packet.h"

u0 *
if_ip_sctp (u0 *packet, u16 size, sctp_association_t *assoc)
{
  if (NULL == assoc || NULL == packet || NULL == assoc->base || NULL == assoc->base->state)
    return NULL;

  if (size <= sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t) + sizeof (sctp_fld_hdr_t))
    return NULL;

  ipv4_t *ip = packet + sizeof (mac_t);
  sctp_t *sctp = packet + sizeof (mac_t) + sizeof (ipv4_t);
  if_ip_sctp_meta_t *m = assoc->base->state;
  if (ip->dest_addr != m->src_ip)
    return NULL;

  u16 check_check_ip = ip->check;
  ip->check = 0;
  if (check_check_ip != (u16)~(u16)ip_checksum ((u16 *)ip, sizeof (ipv4_t)))
    return NULL;

  if (ntohs (ip->len) < sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t) + sizeof (sctp_fld_hdr_t))
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
            if (sctp_process_sctp_init (assoc, (u0 *) ip, ip->len) < 0)
              return NULL;

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
            if (sctp_process_sctp_init_ack (assoc, &sctp->fld + 1, sctp->fld.len) < 0)
              return NULL;

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
        if (sctp_process_sctp_cookie_echo (assoc, &sctp->fld, sctp->fld.len) < 0)
          return NULL;

        break;
      }
    default:
      return NULL;
    }

  return assoc;
}
