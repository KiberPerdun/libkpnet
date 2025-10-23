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

_Noreturn u0 *
recv_packet (u0 *a)
{
  rb_arg_t *arg = a;
  i64 data_size;
  u0 *buffer;

  buffer = malloc (65536);

  for (;;)
    {
      data_size = recv (arg->eth->fd, buffer, 65536, MSG_DONTWAIT);

      if (data_size < 0)
        {
          if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
              usleep(1000);
              continue;
            }

          perror ("recv");
          free (buffer);
          exit (EXIT_FAILURE);
        }

      if (data_size == 0)
        {
          fprintf (stderr, "Connection closed\n");
          free (buffer);
          exit (EXIT_SUCCESS);
        }

      if (push_ringbuf (arg->rb, buffer, data_size) == -1)
        {
          free(buffer);
          usleep(100);
          buffer = malloc (65536);
          continue;
        }

      buffer = malloc (65536);
    }

  free (buffer);
}
