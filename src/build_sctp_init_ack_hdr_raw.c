//
// Created by KiberPerdun on 7/30/25.
//

#include "if_packet.h"

u0 *
build_sctp_init_ack_hdr_raw (u0 *packet, u16 *plen, u32 tag, u32 a_rwnd, u16 os, u16 mis, u32 tsn)
{
  sctp_init_ack_hdr_t *hdr;

  if (NULL == packet || NULL == plen)
    return NULL;

  packet -= sizeof (sctp_init_ack_hdr_t);
  hdr = packet;

  hdr->init_tag = htonl (tag);
  hdr->a_rwnd = htonl (a_rwnd);
  hdr->os = htons (os);
  hdr->mis = htons (mis);
  hdr->init_tsn = htonl (tsn);

  *plen += sizeof (sctp_init_ack_hdr_t);

  return packet;
}
