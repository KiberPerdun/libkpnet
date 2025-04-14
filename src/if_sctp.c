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

  /*
   * Unfortunately, I stopped drinking beer and won’t be able to anymore(((
   * Now only adequate code, to the best of my skills
   */

  printf ("%d\n", size);
  if (sizeof (sctp_cmn_hdr_t) + sizeof (sctp_chunk_fld_t) > size)
    return false;

  pck = packet;
  size = htons (pck->fld.len);

  if (pck->cmn.srcp != args->dstport || pck->cmn.dstp != args->srcport)
    return false;

  pk_check = pck->cmn.check;
  pck->cmn.check = 0;

  printf ("{sctp}(check %u, calc_check %u);\n", pk_check,
          generate_crc32c ((const u0 *) pck, 12 + size));

  fflush (stdout);

  if (pk_check != generate_crc32c ((const u0 *) pck, 12 + size))
    return false;

  ip = args->net_layer.ipv4;

  switch (pck->fld.type)
    {
    case SCTP_INIT:
      {
        if (args->SCTP_STATUS == SCTP_LISTEN)
          {
            args->SCTP_STATUS = SCTP_INIT_RECEIVED;

            args->dstport = pck->cmn.srcp;

            args->sctp_connection.dst_tsn = pck->type.init.init_tag;
            args->sctp_connection.src_ver_tag = pck->type.init.init_tag;

            args->sctp_connection.dest_a_rwnd = pck->type.init.a_rwnd;

            args->sctp_connection.dest_os = ntohs (pck->type.init.os);
            args->sctp_connection.dest_mis = ntohs (pck->type.init.mis);

            memcpy (args->payload, &ip->src_addr, 4);
            memcpy (args->payload + 4, &ip->dest_addr, 4);
            memcpy (args->payload + 8, &args->sctp_connection.dst_ver_tag, 4);
            /* args->sctp_connection.hmac = get_random_u64(); */
            memcpy (args->payload + 12, &args->sctp_connection.hmac, 8);

            return true;
          }

        break;
      }
    case SCTP_INIT_ACK:
      {
        if (pck->cmn.tag != args->sctp_connection.dst_ver_tag)
          return false;

        args->sctp_connection.dst_tsn = pck->type.init_ack.init_tag;
        args->sctp_connection.src_ver_tag = pck->type.init_ack.init_tag;

        args->sctp_connection.dest_a_rwnd = pck->type.init_ack.a_rwnd;

        args->sctp_connection.dest_mis = pck->type.init_ack.mis;
        args->sctp_connection.dest_os = pck->type.init_ack.os;

        args->sctp_connection.hmac
            = *(u32 *)(((u0 *)&(pck->type.init_ack.cookie)) + 4);

        return true;
        break;
      }
    case SCTP_COOKIE_ECHO:
      {
        /* LoL, aeee :)  */
        if (args->SCTP_STATUS == SCTP_INIT_ACK_SENT)
          {
            if (pck->cmn.tag != args->sctp_connection.dst_ver_tag)
              return false;

            if (pck->type.data.tsn != args->sctp_connection.hmac)
              {
                node_t *node = args->sctp_connection.errors;

                if (NULL == node)
                    args->sctp_connection.errors = node = malloc (sizeof (node_t));

                else
                  {
                    for (; NULL != (node = node->next);)
                      ;
                    node = node->next = malloc (sizeof (node_t));
                  }

                node->data = SCTP_ERROR_STALE_COOKIE;
                node->next = NULL;
              }

            return true;
          }

        break;
      }
    case SCTP_COOKIE_ACK:
      {
        return true;
        break;
      }
    }

  return false;
}
