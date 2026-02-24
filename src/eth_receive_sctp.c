//
// Created by KiberPerdun on 12/27/25.
//

#include "if_packet.h"
#include <sys/param.h>

i64 eth_receive_sctp (sctp_association_t *assoc)
{
  sctp_cmn_hdr_t *cmn;
  ringbuf_cell_t *cell;
  u0 *buffer;

  cell = pop_ringbuf (assoc->allocator_2048);
  if (!cell)
    return -1;

  buffer = cell->packet;
  i64 data_size = recv (assoc->eth->fd, buffer, cell->plen, 0);
  if (data_size < 0 || data_size < sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t))
    return -1;

  assoc->current_packet = buffer;
  cmn = assoc->current_packet + sizeof (mac_t) + sizeof (ipv4_t);
  assoc->cursor = cmn + 1;

  return 0;
}
