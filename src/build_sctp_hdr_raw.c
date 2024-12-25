//
// Created by KiberPerdun on 06.12.2024.
//

#include "if_packet.h"
#include "sctp.h"
#include "get_random.h"


bool
build_sctp_hdr_raw (u16 srcp, u16 dstp, u32 tag, SCTP_HDR_TYPE_T type, u16 os, u16 mis, u32 a_rwnd, u8 flags, u0 *_args)
{
  connection_args_t *args;
  sctp_t *sctp;

  u32 random;
  args = _args;
  sctp = args->tp_layer.sctp;
  sctp->cmn.check = 0;

  sctp->cmn.srcp = htons (srcp);
  sctp->cmn.dstp = htons (dstp);

  switch (type)
    {
    case SCTP_INIT:
      {
        sctp->cmn.tag = 0;

        /* If an incorrect header type is specified,
         * there will be no corrupted packet */
        sctp->fld.type = type;
        sctp->fld.flags = 0;
        sctp->fld.len = htons (20);

        sctp->type.init.a_rwnd = htonl (a_rwnd);

        args->sctp_connection.dst_ver_tag = get_random_u32 ();
        sctp->type.init.init_tag = htonl (args->sctp_connection.dst_ver_tag);
        args->sctp_connection.src_tsn = get_random_u32 ();
        sctp->type.init.init_tsn = htonl (args->sctp_connection.src_tsn);

        args->sctp_connection.self_os = os;
        args->sctp_connection.self_mis = mis;
        sctp->type.init.os = htons (os);
        sctp->type.init.mis = htons (mis);

        args->tp_layer.sctp->cmn.check =  (generate_crc32c((const unsigned char *)args->tp_layer.sctp, 12 + ntohs (sctp->fld.len)));

        break;
      }
      /* Must generate mandatory parameter State Cookie (7) */
      /* https://datatracker.ietf.org/doc/html/rfc9260#name-initiation-acknowledgement- */
    case SCTP_INIT_ACK:
      {
        /* Random tag used in association */
        sctp->cmn.tag = args->sctp_connection.src_ver_tag;

        sctp->fld.type = type;
        sctp->fld.flags = 0;
        sctp->fld.len = htons (20 + 8);

        sctp->type.init.a_rwnd = htonl (a_rwnd);
        args->sctp_connection.dst_ver_tag = get_random_u32 ();
        sctp->type.init.init_tag = htonl( args->sctp_connection.dst_ver_tag);
        args->sctp_connection.src_tsn = get_random_u32 ();
        sctp->type.init.init_tsn = htonl (args->sctp_connection.src_tsn);

        args->sctp_connection.self_os = os;
        args->sctp_connection.self_mis = mis;
        sctp->type.init.os = htons (os);
        sctp->type.init.mis = htons (mis);

        ((sctp_opt_t *)(sctp + 1))->type = htons (7);
        ((sctp_opt_t *)(sctp + 1))->len = htons (8);


        u32 cpc = generate_crc32c (args->payload, 20);
        printf ("%u", cpc);
        fflush (stdout);
        memcpy (((u0 *)(sctp + 1)) + sizeof (sctp_opt_t), &cpc, 4);


        args->tp_layer.sctp->cmn.check =  (generate_crc32c ((const u8 *)args->tp_layer.sctp, 12 + ntohs (sctp->fld.len)));

        break;
      }
    case SCTP_DATA:
      break;
    case SCTP_SACK:
      break;
    case SCTP_HEARTBEAT:
      break;
    case SCTP_HEARTBEAT_ACK:
      break;
    case SCTP_ABORT:
      break;
    case SCTP_SHUTDOWN:
      break;
    case SCTP_SHUTDOWN_ACK:
      break;
    case SCTP_ERROR:
      break;
    case SCTP_COOKIE_ECHO:
      break;
    case SCTP_COOKIE_ACK:
      break;
    case SCTP_SHUTDOWN_COMPLETE:
      break;
    }

  return true;
}

