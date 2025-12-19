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
recv_sctp_packet (u0 *a)
{
  ringbuf_cell_t *cell;
  rb_arg_t *arg = a;
  i64 data_size;
  u0 *buffer;

  do
    cell = pop_ringbuf (arg->allocator);

  while (!cell);
  buffer = cell->packet;

  for (;;)
    {
      data_size = recv (arg->eth->fd, buffer, 2048, MSG_DONTWAIT);

      if (data_size < 0)
        {
          if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
              sched_yield ();
              continue;
            }

          perror ("recv");
          push_ringbuf (arg->allocator, buffer, 2048);
          exit (EXIT_FAILURE);
        }

      if (data_size == 0)
        {
          fprintf (stderr, "Connection closed\n");
          push_ringbuf (arg->allocator, buffer, 2048);
          exit (EXIT_SUCCESS);
        }

      if (sctp_check_checksums (buffer, data_size) != 0)
        continue;

      for (; push_ringbuf (arg->rb, buffer, data_size) == -1;)
        sched_yield ();

      do
        cell = pop_ringbuf (arg->allocator);

      while (!cell);
      buffer = cell->packet;
    }
}
