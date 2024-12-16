//
// Created by KiberPerdun on 11.12.2024.
//

#include "if_packet.h"
#include "sctp.h"

bool
if_sctp (u0 *packet, u64 size, connection_args_t *args)
{
  sctp_t *pck;
  ipv4_t *ip;
  u32 pk_check;

  if (size >= sizeof (sctp_t))
    {
      pck = packet;

      /* will be deprecated */

      pk_check = pck->cmn.check;
      pck->cmn.check = 0;

      printf ("{sctp}(check %u, calc_check %u);\n",
              pk_check,
              generate_crc32c ((const unsigned char *)pck, 12 + ntohs (pck->fld.len)));

      fflush (stdout);

      if (pk_check ==  (generate_crc32c ((const unsigned char *)pck, 12 + ntohs (pck->fld.len))))
        {
          ip = args->net_layer.ipv4;

          switch (pck->fld.type)
            {
            case (1):
              {
                args->sctp_connection.src_ver_tag = pck->type.init.init_tag;
                args->sctp_connection.dst_tsn = pck->type.init.init_tag;

                args->sctp_connection.dest_a_rwnd = pck->type.init.a_rwnd;

                args->sctp_connection.dest_os = ntohs (pck->type.init.os);
                args->sctp_connection.dest_mis = ntohs (pck->type.init.mis);

                return true;
                break;
              }
            }

        }
    }

  return false;
}

