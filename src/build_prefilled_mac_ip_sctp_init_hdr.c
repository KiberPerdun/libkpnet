//
// Created by KiberPerdun on 9/4/25.
//

#include "checks.h"
#include "prefilled.h"
#include "random.h"

i32
build_prefilled_mac_ip_sctp_init_hdr (sctp_association_t *assoc)
{
  sctp_ulp_config_t *ulp;
  sctp_cmn_hdr_t *cmn;
  u0 *packet, *tmp_packet;
  ipv4_t *ip;
  u32 chk, plen;
  ringbuf_cell_t *cell;

  struct
  {
    sctp_fld_hdr_t fld;
    sctp_init_hdr_t init;
  } hdr;

  cell = pop_ringbuf (assoc->prefilled_ring);

  if (__builtin_expect (cell->packet == NULL, 0))
    return -1;

  ulp = assoc->ulp;
  packet = cell->packet;
  plen = cell->plen;
  tmp_packet = packet;
  packet += sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t) + sizeof (sctp_fld_hdr_t) + sizeof (sctp_init_hdr_t);

  packet -= (sizeof (sctp_init_hdr_t) + sizeof (sctp_fld_hdr_t));
  hdr.init.init_tag = get_random_from_buffer_u32 ();
  hdr.init.a_rwnd = htonl (ulp->src_arwnd);
  hdr.init.os = htons (ulp->src_os);
  hdr.init.mis = htons (ulp->src_mis);
  hdr.init.init_tsn = htonl (ulp->src_os);

  hdr.fld.type = SCTP_INIT;
  hdr.fld.flags = 0;
  hdr.fld.len = htons (sizeof (sctp_init_hdr_t) + sizeof (sctp_fld_hdr_t));

  memcpy (packet, &hdr, sizeof (sctp_init_hdr_t) + sizeof (sctp_fld_hdr_t ));
  plen += sizeof (sctp_init_hdr_t) + sizeof (sctp_fld_hdr_t );

  cmn = packet - sizeof (sctp_cmn_hdr_t);
  ip = (u0 *) cmn - sizeof (ipv4_t);
  chk = ip->check;
  ip->len = htons (plen - sizeof (mac_t));
  chk += htons (0) - ip->len;
  if (chk > 0xFFFF)
    chk = (chk & 0xFFFF) + (chk >> 16);
  ip->check = chk;

  cmn->check = ~(u32) generate_crc32c_on_crc32c ((const u8 *) cmn, sizeof (sctp_cmn_hdr_t ) + sizeof (sctp_fld_hdr_t) + sizeof (sctp_init_hdr_t), 0xFFFFFFFF);

  packet = tmp_packet;
  push_ringbuf (assoc->tx_ring, packet, plen);
  push_ringbuf (assoc->prefilled_ring, cell->packet, cell->plen);
  return 0;
}
