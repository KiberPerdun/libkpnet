//
// Created by KiberPerdun on 14.11.24.
//

#include "if_packet.h"

u0
recv_filtered (i32 fd, lrcall_t filter, connection_args_t *args)
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
          if (filter (buf_cp, data_size, args))
            {
              printf ("%d", args->TCP_STATUS);
              break;
            }

          //puts ("not aeee");
        }
    }

  free (buffer);
}
