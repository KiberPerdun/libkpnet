//
// Created by KiberPerdun on 2/21/25.
//

#include "checks.h"
#include "if_packet.h"
#include "random.h"

inline __attribute__ ((always_inline, hot)) u0 *
build_ip_raw (u0 *packet, u16 *plen, u32 src, u32 dst, u8 proto)
{
  ipv4_t hdr;

  if (NULL == packet || NULL == plen)
    return NULL;

  packet -= sizeof (ipv4_t);

  hdr.ihl = 5;
  hdr.ver = 4;
  hdr.tos = htons (0x2);
  hdr.indent = get_random_u32 ();
  hdr.offset = 0;
  hdr.ttl = 181;
  hdr.proto = proto;
  hdr.src_addr = src;
  hdr.dest_addr = dst;
  hdr.check = 0;
  *plen += sizeof (ipv4_t);
  hdr.len = htons (*plen);

  memcpy (packet, &hdr, sizeof (ipv4_t));

  ((ipv4_t *) packet)->check = ~(u16)ip_checksum ((u16 *) &hdr, sizeof (ipv4_t));

  return packet;
}
