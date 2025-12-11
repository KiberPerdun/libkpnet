//
// Created by KiberPerdun on 3/27/25.
//

#include "checks.h"
#include "if_packet.h"

u0 *
if_ip_sctp (u0 *packet, u16 size, sctp_association_t *assoc)
{
  if (NULL == assoc || NULL == packet || NULL == assoc->base
      || NULL == assoc->base->state)
    return NULL;

  ipv4_t *ip = packet + sizeof (mac_t);
  sctp_t *sctp = packet + sizeof (mac_t) + sizeof (ipv4_t);
  if (ip->dest_addr != assoc->ulp->src_ip)
    return NULL;

  if (assoc->ulp->src_port != ntohs (sctp->cmn.dstp))
    return NULL;

  puts ("SCTP packet received");
  printf ("%d\n", sctp->fld.type);

  switch (sctp->fld.type)
    {
    case SCTP_INIT:
      {
        if (assoc->status == SCTP_LISTEN)
          {
            if (sctp_process_sctp_init (assoc, (u0 *)ip, ip->len) < 0)
              return NULL;
          }
        else
          return NULL;

        break;
      }
    case SCTP_INIT_ACK:
      {
        if (assoc->status == SCTP_INIT_SENT)
          {
            if (sctp_process_sctp_init_ack (assoc, &sctp->fld + 1,
                                            sctp->fld.len)
                < 0)
              return NULL;
          }
        else
          return NULL;

        break;
      }
    case SCTP_COOKIE_ECHO:
      {
        if (assoc->status == SCTP_LISTEN)
          {
            if (sctp_process_sctp_cookie_echo (assoc, &sctp->fld,
                                               sctp->fld.len)
                < 0)
              return NULL;
          }
        else
          return NULL;

        break;
      }
    case SCTP_COOKIE_ACK:
      {
        if (assoc->status == SCTP_INIT_ACK_RECEIVED)
          {
            if (sctp_process_sctp_cookie_ack (assoc, &sctp->fld, sctp->fld.len)
                < 0)
              return NULL;
          }
        else
          return NULL;

        break;
      }
    default:
      return NULL;
    }

  puts ("passed");
  return assoc;
}
