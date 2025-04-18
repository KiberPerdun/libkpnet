//
// Created by KiberPerdun on 3/12/25.
//

#include "if_packet.h"
#include "linux/io_uring.h"
#include "sys/syscall.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/io_uring.h>
#include <sys/syscall.h>
#include <sys/mman.h>

#define QUEUE_DEPTH 128

#define BUFFER_SIZE 1024

/* TODO: io_uring */

u0
recv_packet (i32 fd, ifcall filter, u0 *meta)
{
  i64 data_size;
  u0 *buffer;
  u0 *buf_cp;

  buffer = malloc (65536);

  for (;;)
    {
      data_size = recv (fd, buffer , 65536, 0);
      buf_cp = buffer;
      if (data_size > 46)
        {
          if (filter (buf_cp, data_size, meta))
            {
              break;
            }
        }
    }

  free (buffer);
}
