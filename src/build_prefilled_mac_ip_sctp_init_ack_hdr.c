//
// Created by KiberPerdun on 10/2/25.
//

#include "checks.h"
#include "prefilled.h"
#include "random.h"

i32
build_prefilled_mac_ip_sctp_init_ack_hdr (sctp_association_t *assoc)
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
    sctp_init_ack_hdr_t init;
    sctp_cookie_t cookie_hdr;
    cookie_t cookie;
    u8 hmac[HMAC_MD5_KEY_LEN];
  } hdr;

  cell = pop_ringbuf (assoc->prefilled_ring);

  if (__builtin_expect (cell->packet == NULL, 0))
    return -1;

  ulp = assoc->ulp;
  packet = cell->packet;
  plen = cell->plen;
  tmp_packet = packet;
  packet += sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t) + sizeof (hdr);

  packet -= sizeof (hdr);
  hdr.init.init_tag = get_random_from_buffer_u32 ();
  hdr.init.a_rwnd = htonl (ulp->src_arwnd);
  hdr.init.os = htons (ulp->src_os);
  hdr.init.mis = htons (ulp->src_mis);
  hdr.init.init_tsn = get_random_from_buffer_u32 ();

  hdr.fld.type = SCTP_INIT_ACK;
  hdr.fld.flags = 0;
  hdr.fld.len = htons (sizeof (hdr));

  hdr.cookie_hdr.type = htons (7);
  hdr.cookie_hdr.len = htons (sizeof (sctp_cookie_t) + sizeof (cookie_t) + HMAC_MD5_KEY_LEN);

  hdr.cookie.dst_port = ulp->src_port;
  hdr.cookie.src_port = assoc->dst_port;
  hdr.cookie.src_ip = assoc->dst_ip;
  hdr.cookie.dst_ip = ulp->src_ip;
  hdr.cookie.os = assoc->os;
  hdr.cookie.mis = assoc->mis;
  hdr.cookie.tsn = assoc->tsn;
  hdr.cookie.dst_tsn = assoc->dst_tsn;
  hdr.cookie.a_rwnd = assoc->a_rwnd;
  hdr.cookie.init_tag = get_random_u32 ();
  hdr.cookie.ver_tag = assoc->ver_tag;

  u8 key[16] = {
    0xC9, 0xAB, 0x02, 0x9C, 0x92, 0xBF, 0x47, 0xF3,
    0xC9, 0xAB, 0x02, 0x9C, 0x92, 0xBF, 0x47, 0xF3
  };

  hmac_md5 ((u8 *) &hdr.cookie, sizeof (hdr.cookie), key, HMAC_MD5_KEY_LEN, (u0 *) &hdr.hmac);
  memcpy (packet, &hdr, sizeof (hdr));
  plen += sizeof (hdr);

  cmn = packet - sizeof (sctp_cmn_hdr_t);
  ip = (u0 *) cmn - sizeof (ipv4_t);
  ip->dest_addr = assoc->dst_ip;
  chk = ip->check;
  ip->len = htons (plen - sizeof (mac_t));
  chk += htons (0) - ip->len - ip->dest_addr;
  if (chk > 0xFFFF)
    chk = (chk & 0xFFFF) + (chk >> 16);
  ip->check = chk;

  cmn->dstp = assoc->dst_port;
  cmn->tag = assoc->ver_tag;
  cmn->check = 0;
  cmn->check = ~(u32) generate_crc32c_on_crc32c ((const u8 *) cmn, sizeof (sctp_cmn_hdr_t) + ntohs (hdr.fld.len), 0xFFFFFFFF);

  packet = tmp_packet;
  push_ringbuf (assoc->tx_ring, packet, plen);
  push_ringbuf (assoc->prefilled_ring, cell->packet, cell->plen);
  return 0;
}
