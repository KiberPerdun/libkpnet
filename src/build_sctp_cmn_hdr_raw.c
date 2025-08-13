//
// Created by KiberPerdun on 3/14/25.
//

#include "if_packet.h"

inline __attribute__ ((always_inline, hot)) u0 *
build_sctp_cmn_hdr_raw (u0 *packet, u16 *plen, u16 srcport, u16 dstport, u32 tag)
{
  sctp_cmn_hdr_t *hdr;

  if (NULL == packet || NULL == plen)
    return NULL;

  packet -= sizeof (sctp_cmn_hdr_t);
  hdr = packet;

  hdr->check = 0;
  hdr->srcp = htons (srcport);
  hdr->dstp = htons (dstport);
  hdr->tag = tag;

  *plen += sizeof (sctp_cmn_hdr_t);

  hdr->check = generate_crc32c (packet, *plen);

  return packet;
}
