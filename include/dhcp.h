//
// Created by KiberPerdun on 3/1/25.
//

#ifndef DHCP_H
#define DHCP_H

#include "if_packet.h"

#define OP_DHCP_BOOTREQUEST 1
#define OP_DHCP_BOOTREPLY   2

#define OP_HTYPE_10MBETHRNET 1

typedef struct dhcp_hdr
{
  u8 op;
  u8 htype;
  u32 xid;
  u16 secs;
  u16 flags;
  u32 ciaddr;
  u32 yiaddr;
  u32 siaddr;
  u8 chaddr[16];
  u8 sname[64];
  u8 file[128];
} dhcp_t;

frame_data_t *build_dhcp_bootrequest (frame_data_t *frame);

#endif //DHCP_H
