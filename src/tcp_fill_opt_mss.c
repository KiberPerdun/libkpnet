//
// Created by KiberPerdun on 22.10.24.
//

#include "tcp.h"

tcp_opt_mss_t *
tcp_fill_opt_mss (u16 mss)
{
  tcp_opt_mss_t *hdr = malloc (sizeof (tcp_opt_mss_t));
  hdr->opt.type = 2;
  hdr->opt.len = 4;
  hdr->mss = htons (mss);
  return hdr;
}
