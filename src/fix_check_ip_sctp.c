//
// Created by KiberPerdun on 3/14/25.
//

#include "checks.h"

frame_data_t *
fix_check_ip_sctp (frame_data_t *frame, u16 frm_max_len)
{
  u0 *packet;
  ipv4_t *ip;
  sctp_t *sctp;

  if (NULL == frame->packet)
    return frame;

  packet = frame->packet - (frm_max_len - frame->plen) + sizeof (mac_t);
  sctp = packet + sizeof (ipv4_t);
  ip = packet;

  ip->check = 0;
  ip->check = ~(u16)ip_checksum ((u16 *)ip, sizeof (ipv4_t));
  sctp->cmn.check = generate_crc32c ((u8 *)sctp, 12 + ntohs (sctp->fld.len));

  return frame;
}
