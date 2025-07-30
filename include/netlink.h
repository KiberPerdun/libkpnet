//
// Created by KiberPerdun on 7/17/25.
//

#ifndef NETLINK_H
#define NETLINK_H

#include "if_packet.h"

i32 netlink_open ();
i32 netlink_close (i32 fd);
/* TODO: add error checks */
i32 create_veth_pair (const char *src, const char *dst);
i32 get_ifid (const char *name);
i32 get_ifmac (const char *name, u8 *mac);
i32 delete_if (i32 intf_index);
i32 up_if (i32 intf_index);
i32 bind_if (i32 intf_index, const u32 ip);

#endif //NETLINK_H
