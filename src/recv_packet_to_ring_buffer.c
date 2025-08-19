//
// Created by KiberPerdun on 8/18/25.
//

#include "if_packet.h"
#include "ring_buffer.h"

#include <errno.h>

u0 *
recv_packet_to_ring_buffer (u0 *eth_)
{
  eth_t *eth = (eth_t *) eth_;

  if (!eth || !eth->rb || eth->fd < 0)
    return NULL;

  ring_buffer_t *rb = eth->rb;
  i32 fd = eth->fd;
  u0 *buffer;

  int flags = fcntl (fd, F_GETFL, 0);
  fcntl (fd, F_SETFL, flags & ~O_NONBLOCK);

  for (;;)
    {
      if ((buffer = get_next_address_ring_buffer_producer (rb)))
        {
          i64 data_size = recv (fd, buffer, 2048, 0);
          if (data_size < 0)
            {
              perror ("recv");
              return NULL;
            }
        }

      else
        usleep (10000);
    }
}
