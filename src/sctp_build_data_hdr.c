//
// Created by KiberPerdun on 12/11/25.
//

#include "if_packet.h"
#include <sys/param.h>

u64
sctp_build_data_hdr (sctp_association_t *assoc, u16 stream_id)
{
  ringbuf_cell_t *cell = NULL;
  /* TODO! error handling */
  if (assoc->current_packet == NULL)
    return 0;

  struct
  {
    sctp_fld_hdr_t fld;
    sctp_data_hdr_t data;
  } hdr;

  if (assoc->remain_plen < sizeof (hdr) + 4)
    {
    flush:
      push_ringbuf (assoc->tx_ring, assoc->current_packet, assoc->mtu - assoc->remain_plen + (sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t)));
      assoc->remain_plen = assoc->mtu;
      cell = pop_ringbuf (assoc->prefilled_ring);
      assoc->current_packet = cell->packet;
      /* TODO! делегировать другим уровням абстракции */
      assoc->cursor = cell->packet + sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t);
    }

  u0 *packet;
  u32 pplen; /* сколько мы можем занять */
  u32 alligned_plen;
  sctp_cmn_hdr_t *cmn;
  cmn = assoc->current_packet + sizeof (mac_t) + sizeof (ipv4_t);
  sctp_thread_t *thread = assoc->os_threads[stream_id];
  pplen = MIN (assoc->remain_plen - sizeof (hdr), thread->pos_high - thread->pos_current);
  if (pplen == 0)
    goto flush;

  alligned_plen = (pplen + 3) & ~3;

  if (alligned_plen > assoc->remain_plen - sizeof (hdr))
    {
      pplen &= ~3;
      alligned_plen = pplen;
    }

  packet = assoc->cursor;

  hdr.fld.type = SCTP_DATA;
  hdr.fld.flags = thread->flags;

  hdr.data.tsn = assoc->tsn;
  assoc->tsn += htonl (1);

  hdr.data.so = htons (stream_id);
  hdr.data.sn = thread->ssn;
  hdr.data.proto_def = thread->proto;

  if (thread->pos_high - thread->pos_low <= pplen)
    hdr.fld.flags |= SCTP_UNFRAGMENTED;

  else if (thread->pos_low == thread->pos_current)
    hdr.fld.flags |= SCTP_BBIT;

  else if (thread->pos_current + MIN (pplen, thread->pos_high - thread->pos_current) == thread->pos_high)
    hdr.fld.flags |= SCTP_EBIT;

  hdr.fld.len = htons (pplen);
  memcpy (packet, &hdr, sizeof (hdr));
  memcpy (packet + sizeof (hdr), thread->buffer + thread->pos_current, pplen);
  memset (packet + sizeof (hdr) + pplen, 0, alligned_plen - pplen);
  assoc->cursor += alligned_plen;
  assoc->remain_plen -= alligned_plen;

  sctp_chunk_slot_t *slot;
  slot = &assoc->rtx[ntohl (assoc->tsn) & RTX_MASK];
  slot->tsn = ntohl (assoc->tsn);
  slot->buffer = thread->buffer + thread->pos_current;
  thread->pos_current += pplen - sizeof (hdr);
  slot->len = pplen;
  slot->stream_id = stream_id;
  slot->acked = 0;
  slot->in_use = 1;
  slot->retry_count = 2;

  cmn->tag = assoc->ver_tag;

  return 0;
}
