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

  if (m->src_port != sctp->cmn.dstp)
    return NULL;

  puts ("SCTP packet received");

  switch (sctp->fld.type)
    {
    case SCTP_INIT:
      {
        if (m->state == SCTP_LISTEN)
          {
            m->dst_port = sctp->cmn.srcp;
            m->state = SCTP_INIT_RECEIVED;

            m->dst_tsn = sctp->type.init.init_tag;
            m->src_ver_tag = sctp->type.init.init_tag;
            m->dst_arwnd = sctp->type.init.a_rwnd;
            m->dst_os = ntohs (sctp->type.init.os);
            m->dst_mis = ntohs (sctp->type.init.mis);

            m->dst_ip = ip->src_addr;
          }
        break;
      }
    case SCTP_INIT_ACK:
      {
        if (m->state == SCTP_INIT_SENT)
          {
            m->dst_port = sctp->cmn.srcp;
            m->state = SCTP_INIT_ACK_RECEIVED;

            m->src_ver_tag = sctp->type.init_ack.init_tag;
            m->dst_arwnd = sctp->type.init_ack.a_rwnd;
            m->dst_os = ntohs (sctp->type.init_ack.os);
            m->dst_mis = ntohs (sctp->type.init_ack.mis);
            m->src_tsn = sctp->type.init_ack.init_tsn;


          }
      }
    default:
      return NULL;
    }

  return meta;
}
