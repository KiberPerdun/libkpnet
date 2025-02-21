//
// Created by KiberPerdun on 2/21/25.
//

#include "if_packet.h"

frame_data_t *
fix_check_ip (frame_data_t *frame, u16 opt_len)
{
  ipv4_t *ip;

  if (NULL == frame->packet)
    return frame;

  ip = frame->packet;
  ip->check = 0;
  ip->check = in_check ((u16 *)ip, sizeof (ipv4_t) + opt_len);

  return frame;
}

