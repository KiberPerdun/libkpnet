//
// Created by KiberPerdun on 12/8/25.
//

#include "checks.h"
#include "sctp.h"

i64
sctp_check_checksums (u0 *packet, u64 plen)
{
  if (plen < sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t ))
    return -1;

  ipv4_t *ip = packet + sizeof (mac_t);
  sctp_t *sctp = packet + sizeof (mac_t) + sizeof (ipv4_t);

  u16 check_check_ip = ip->check;
  ip->check = 0;
  if (check_check_ip != (u16) ~(u16)ip_checksum ((u16 *)ip, sizeof (ipv4_t)))
    return -1;

  /* TODO! remove */
  ip->check = check_check_ip;

  if (ntohs (ip->len)
      < sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t) + sizeof (sctp_fld_hdr_t))
    return -1;

  ~(u32) generate_crc32c_on_crc32c ((const u8 *) sctp, sizeof (sctp_cmn_hdr_t ) + ntohs (sctp->fld.len), 0xFFFFFFFF);

  u32 check_check_sctp = sctp->cmn.check;
  sctp->cmn.check = 0;

  if (check_check_sctp
      !=       ~(u32) generate_crc32c_on_crc32c ((const u8 *) sctp, sizeof (sctp_cmn_hdr_t ) + ntohs (sctp->fld.len), 0xFFFFFFFF))
    return -1;

  /* TODO! remove */
  sctp->cmn.check = check_check_sctp;

  return 0;
}
