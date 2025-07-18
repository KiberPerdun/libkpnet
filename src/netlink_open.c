//
// Created by KiberPerdun on 7/17/25.
//

#include "netlink.h"

i32 netlink_open ()
{
  i32 fd;

  if (fd = socket (AF_NETLINK, SOCK_RAW, NETLINK_ROUTE), fd < 0)
    {
    perror ("netlink socket");
    return -1;
  }

  return fd;
}
