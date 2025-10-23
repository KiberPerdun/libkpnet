//
// Created by KiberPerdun on 10/9/25.
//

#include "if_packet.h"
#include <sys/param.h>

i32
build_prefilled_mac_ip_sctp_data_hdr (sctp_association_t *assoc, u16 stream_id)
{
  ringbuf_cell_t *cell;
  sctp_thread_t *thread = assoc->os_threads[stream_id];
  u0 *packet, *tmp_packet;
  u32 chk, plen, pplen;
  sctp_cmn_hdr_t *cmn;
  ipv4_t *ip;

  struct
  {
    sctp_fld_hdr_t fld;
    sctp_data_hdr_t data;
  } hdr;

  hdr.fld.type = SCTP_DATA;
  hdr.fld.flags = thread->flags;

  hdr.data.tsn = assoc->tsn;
  pthread_spin_lock (&assoc->lock);
  assoc->tsn += htonl (1);
  pthread_spin_unlock (&assoc->lock);

  hdr.data.so = htons (stream_id);
  hdr.data.sn = thread->ssn;
  hdr.data.proto_def = thread->proto;

  cell = pop_ringbuf (assoc->prefilled_ring);
  pplen = MAX_PACKET_LEN - (sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t) + sizeof (hdr) + 16);
  plen = cell->plen;
  if (thread->pos_high - thread->pos_low <= pplen)
    hdr.fld.flags |= SCTP_UNFRAGMENTED;

  else if (thread->pos_low == thread->pos_current)
    hdr.fld.flags |= SCTP_BBIT;

  else if (thread->pos_current + MIN (pplen, thread->pos_high - thread->pos_current) == thread->pos_high)
    hdr.fld.flags |= SCTP_EBIT;

  packet = cell->packet;
  packet += sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t);

  hdr.fld.len = htons (sizeof (hdr) + MIN (pplen, thread->pos_high - thread->pos_current));
  memcpy (packet, &hdr, sizeof (hdr));
  memcpy (packet + sizeof (hdr), thread->buffer + thread->pos_current, MIN (pplen, thread->pos_high - thread->pos_current));
  plen += sizeof (hdr) + MIN (pplen, thread->pos_high - thread->pos_current);
  thread->pos_current += MIN (pplen, thread->pos_high - thread->pos_current);

  cmn = packet - sizeof (sctp_cmn_hdr_t);
  ip = (u0 *) cmn - sizeof (ipv4_t);
  chk = ip->check;
  ip->len = htons (plen - sizeof (mac_t));
  chk += htons (0) - ip->len;
  if (chk > 0xFFFF)
    chk = (chk & 0xFFFF) + (chk >> 16);
  ip->check = chk;

  cmn->check = ~(u32) generate_crc32c_on_crc32c ((const u8 *) cmn, sizeof (sctp_cmn_hdr_t ) + ntohs (hdr.fld.len), 0xFFFFFFFF);

  printf ("%d", plen);
  puts (" ");
  push_ringbuf (assoc->tx_ring, cell->packet, plen);
  push_ringbuf (assoc->prefilled_ring, cell->packet, cell->plen);

  return 0;
}
