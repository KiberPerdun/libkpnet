//
// Created by KiberPerdun on 3/12/25.
//

#include "if_packet.h"
#include "linux/io_uring.h"
#include "sys/syscall.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/io_uring.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <unistd.h>

u0
recv_packet (i32 fd, ifcall filter, u0 *meta)
{
  i64 data_size;
  u0 *buffer;

  buffer = malloc (65536);

  for (;;)
    {
      data_size = recv (fd, buffer, 65536, MSG_DONTWAIT);
      if (data_size < 0)
        {
          if (EAGAIN == errno || EWOULDBLOCK == errno)
              continue;

          perror ("recv");
          free (buffer);
          return;
        }

      if (filter (buffer, data_size, meta))
          break;
    }

  free (buffer);
}
