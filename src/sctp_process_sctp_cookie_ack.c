//
// Created by KiberPerdun on 10/3/25.
//

#include "if_packet.h"
#include "prefilled.h"

i64
sctp_process_sctp_cookie_ack (sctp_association_t *assoc, u0 *packet, u32 plen)
{
  if (ntohs (plen) < sizeof (sctp_fld_hdr_t) + sizeof (sctp_cmn_hdr_t ))
    return -1;

  pthread_spin_lock (&assoc->lock);
  assoc->status = SCTP_ESTABLISHED;
  pthread_spin_unlock (&assoc->lock);

  return 0;
}

