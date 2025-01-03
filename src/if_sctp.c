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

                memcpy (args->payload, &ip->src_addr, 4);
                memcpy (args->payload + 4, &ip->dest_addr, 4);
                memcpy (args->payload + 8, &args->sctp_connection.dst_ver_tag, 4);
                // args->sctp_connection.hmac = get_random_u64();
                memcpy (args->payload + 12, &args->sctp_connection.hmac, 8);

                return true;
                break;
              }
            case (2):
              {
                args->sctp_connection.src_ver_tag = pck->type.init.init_tag;
                args->sctp_connection.dst_tsn = pck->type.init.init_tag;

                args->sctp_connection.dest_a_rwnd = pck->type.init.a_rwnd;

                args->sctp_connection.dest_os = ntohs (pck->type.init.os);
                args->sctp_connection.dest_mis = ntohs (pck->type.init.mis);

                args->sctp_connection.hmac = *((u32 *)(((u0 *)(&pck->type.init + 1)) + 8));

                return true;
                break;
              }
            }

        }
    }

  return false;
}

