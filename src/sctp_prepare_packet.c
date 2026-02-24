//
// Created by KiberPerdun on 12/27/25.
//

#include "if_packet.h"

i64
sctp_prepare_packet (sctp_association_t *assoc)
{
  assoc->xdp_seg_count = 0;
  assoc->xdp_segs[0] = (u0 *) (assoc->prefilled_umem_packet - assoc->xdp->umem);
  assoc->xdp_seg_lens[0] = assoc->prefilled_umem_packet_len;
  ++assoc->xdp_seg_count;
  assoc->remain_plen = assoc->mtu;

  return 0;
}
