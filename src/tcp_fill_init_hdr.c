//
// Created by KiberPerdun on 22.10.24.
//

#include "tcp.h"
#include <arpa/inet.h>


struct pseudo_header {
  uint32_t source_address;
  uint32_t dest_address;
  uint8_t placeholder;
  uint8_t protocol;
  uint16_t tcp_length;
};

tcp_t *
tcp_fill_init_hdr (u16 src, u16 dst, u16 flags)
{
  tcp_t *hdr = malloc (sizeof (tcp_t));
  hdr->srcp = htons (src);
  hdr->dstp = htons (dst);
  hdr->seq = 0;
  hdr->ack = 0;
  hdr->flags = htons ((5 + (4 / 4)) << 12 | flags);
  hdr->win = htons (64240);
  hdr->check = 0;
  hdr->urgent = 0;
  return hdr;
}
