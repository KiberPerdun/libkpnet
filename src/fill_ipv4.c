#include "ipv4.h"
#include "malloc.h"

ipv4_t *
fill_ipv4(u32 src, u32 dst)
{
  ipv4_t *hdr = malloc (sizeof (ipv4_t));
  hdr->ihl = 5;
  hdr->ver = 4;
  hdr->tos = 0;
  hdr->len = 0;
  hdr->indent = 0;
  hdr->offset = 0;
  hdr->ttl = 220;
  hdr->proto = PROTO_SCTP;
  hdr->checksum = 0;
  hdr->src_addr = src;
  hdr->dest_addr = dst;
  return hdr;
}
