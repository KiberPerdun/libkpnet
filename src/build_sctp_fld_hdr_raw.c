//
// Created by KiberPerdun on 3/14/25.
//

#include "if_packet.h"

inline __attribute__ ((always_inline, hot)) u0 *
build_sctp_fld_hdr_raw (u0 *packet, u16 *plen, u8 type, u8 flags, u16 len)
{
  sctp_fld_hdr_t hdr;

  if (NULL == packet || NULL == plen)
    return NULL;

  packet -= sizeof (sctp_fld_hdr_t);

  hdr.type = type;
  hdr.flags = flags;
  hdr.len = htons (len);

  memcpy (packet, &hdr, sizeof (sctp_fld_hdr_t));

  *plen += sizeof (sctp_fld_hdr_t);

  return packet;
}
