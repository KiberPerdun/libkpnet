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
  u0 *packet;

  struct
  {
    sctp_fld_hdr_t fld;
    sctp_init_hdr_t init;
  } hdr;

  ulp = assoc->ulp;
  packet = assoc->current_packet;
  packet += sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t) + sizeof (hdr);

  packet -= sizeof (hdr);
  hdr.init.init_tag = get_random_from_buffer_u32 ();
  hdr.init.a_rwnd = htonl (ulp->src_arwnd);
  hdr.init.os = htons (ulp->src_os);
  hdr.init.mis = htons (ulp->src_mis);
  hdr.init.init_tsn = get_random_from_buffer_u32 ();

  hdr.fld.type = SCTP_INIT;
  hdr.fld.flags = 0;
  hdr.fld.len = htons (sizeof (hdr));

  memcpy (packet, &hdr, sizeof (hdr));

  assoc->remain_plen -= sizeof (hdr);
  eth_send_sctp (assoc);
  sctp_prepare_packet (assoc);

  return 0;
}
