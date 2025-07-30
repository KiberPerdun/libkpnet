//
// Created by KiberPerdun on 7/31/25.
//

#include "netlink.h"

i32
get_ifmac (const char *name, u8 *mac)
{
  return get_iftype (name, IFLA_ADDRESS, mac, 6) < 6;
}
