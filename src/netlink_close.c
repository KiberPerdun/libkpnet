//
// Created by KiberPerdun on 7/17/25.
//

#include "netlink.h"

i32 netlink_close (i32 fd)
{
  if (close (fd) < 0)
    {
      perror ("netlink close");
      return -1;
    }

  return 0;
}
