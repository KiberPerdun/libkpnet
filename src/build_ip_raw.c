//
// Created by KiberPerdun on 2/21/25.
//

#include "checks.h"
#include "get_random.h"
#include "if_packet.h"

frame_data_t *
build_ip_raw (frame_data_t *frame, u32 src, u32 dst, u8 proto)
{
  ipv4_t *hdr;

  if (frame->plen < sizeof (ipv4_t) || NULL == frame->packet)
    {
      frame->plen = 0;
      return frame;
    }

  hdr = frame->packet;

  hdr->ihl = 5;
  hdr->ver = 4;
  hdr->tos = htons (0x2);
  hdr->indent = get_random_u32 ();
  hdr->offset = 0;
  hdr->ttl = 181;
  hdr->proto = proto;
  hdr->src_addr = src;
  hdr->dest_addr = dst;
  hdr->check = 0;

  if (frame->sync)
    {
      hdr->len = 0;
      ((frame_sync_ip_tcp_t *)frame->sync)->ip_check_part = ip_checksum ((u16 *)hdr, sizeof (ipv4_t));
    }

  hdr->len = htons (40);

  frame->packet += sizeof (ipv4_t);
  frame->plen -= sizeof (ipv4_t);

  return frame;
}
