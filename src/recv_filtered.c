//
// Created by KiberPerdun on 14.11.24.
//

#include "if_packet.h"

u0
recv_filtered (i32 fd, lrcall_t filter, packet_args_t *args)
{
  i64 data_size;
  u0 *buffer;

  buffer = malloc (65536);

  for (;;)
    {
      data_size = recv (fd, buffer , 65536, 0);
      if (data_size > 46)
        {
          if (!filter (buffer, data_size, args))
            {
              continue;
            }

          //puts ("not aeee");
        }
    }

  free (buffer);
}
