//
// Created by KiberPerdun on 12/27/25.
//

#include "if_packet.h"

i64
sctp_prepare_packet (sctp_association_t *assoc)
{
  ringbuf_cell_t *cell;
  cell = pop_ringbuf (assoc->prefilled_ring);
  if (!cell)
    return -1;

  assoc->current_packet = cell->packet;
  assoc->cursor = assoc->current_packet + sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t );
  assoc->remain_plen = assoc->mtu;

  return 0;
}
