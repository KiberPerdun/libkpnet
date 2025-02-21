//
// Created by KiberPerdun on 06.12.2024.
//

#include "if_packet.h"
#include "sctp.h"
#include "get_random.h"


bool
build_sctp_hdr_raw (u16 srcp, u16 dstp, u32 tag, SCTP_HDR_TYPE_T type, u16 os,
                    u16 mis, u32 a_rwnd, u8 flags, u0 *_args)
{
  connection_args_t *args;
  sctp_t *sctp;

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
        sctp->fld.flags = 0;
        sctp->fld.type = type;
        sctp->fld.len = htons (20);

        sctp->type.init.a_rwnd = htonl (a_rwnd);

        sctp->type.init.os = args->sctp_connection.self_os = htons (os);
        sctp->type.init.mis = args->sctp_connection.self_mis = htons (mis);
        sctp->type.init.init_tsn = args->sctp_connection.src_tsn = get_random_u32 ();
        sctp->type.init.init_tag = args->sctp_connection.dst_ver_tag = get_random_u32 ();

        args->tp_layer.sctp->cmn.check =  (generate_crc32c((const unsigned char *)args->tp_layer.sctp, 12 + ntohs (sctp->fld.len)));

        break;
      }
      /* Must generate mandatory parameter State Cookie (7) */
      /* https://datatracker.ietf.org/doc/html/rfc9260#name-initiation-acknowledgement- */
    case SCTP_INIT_ACK:
      {
        /* Random tag used in association */
        sctp->cmn.tag = args->sctp_connection.src_ver_tag;

        sctp->fld.flags = 0;
        sctp->fld.type = type;
        sctp->fld.len = htons (20 + 8);
        sctp->type.init_ack.a_rwnd = htonl (a_rwnd);

        sctp->type.init_ack.os = args->sctp_connection.self_os = htons (os);
        sctp->type.init_ack.mis = args->sctp_connection.self_mis = htons (mis);
        sctp->type.init_ack.init_tsn = args->sctp_connection.src_tsn = get_random_u32 ();
        sctp->type.init_ack.init_tag = args->sctp_connection.dst_ver_tag = get_random_u32 ();

        sctp->type.init_ack.cookie.len = htons (8);
        sctp->type.init_ack.cookie.type = htons (7);

        *(u32 *)(((u0 *)&(sctp->type.init_ack.cookie)) + 4) = args->sctp_connection.hmac = generate_crc32c (args->payload, 20);

        args->tp_layer.sctp->cmn.check = (generate_crc32c ((const u8 *)args->tp_layer.sctp, 12 + ntohs (sctp->fld.len)));

        break;
      }
    case SCTP_COOKIE_ECHO:
      {
        sctp->cmn.tag = args->sctp_connection.src_ver_tag;

        sctp->fld.flags = 0;
        sctp->fld.type = type;
        sctp->fld.len = htons (8);

        sctp->type.data.tsn = args->sctp_connection.hmac;

        args->tp_layer.sctp->cmn.check = (generate_crc32c ((const u8 *)args->tp_layer.sctp, 12 + ntohs (sctp->fld.len)));

        break;
      }
    case SCTP_COOKIE_ACK:
      {
        sctp->cmn.tag = args->sctp_connection.src_ver_tag;

        sctp->fld.flags = 0;
        sctp->fld.type = type;
        sctp->fld.len = htons (4);

        args->tp_layer.sctp->cmn.check = (generate_crc32c ((const u8 *)args->tp_layer.sctp, 12 + ntohs (sctp->fld.len)));

        break;
      }
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
      {
        if (NULL == args->sctp_connection.errors)
          return false;
      }
    case SCTP_DATA:
      break;
    case SCTP_SHUTDOWN_COMPLETE:
      break;
    }

  return true;
}

