//
// Created by KiberPerdun on 2/14/25.
//

#include "checks.h"

frame_data_t *fix_check (frame_data_t *frame)
{
  if (PROTO_STACK_IP_TCP == frame->proto)
    return fix_check_ip_tcp (frame, 0);

  return frame;
}
