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
  // printf ("%d", *(u8 *)ip); // 01000101
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

  switch (sctp->fld.type)
    {
    case (SCTP_INIT):
      {
        m->state = SCTP_INIT_RECEIVED;
        break;
      }
    default:
      return NULL;
    }

  return meta;
}
