//
// Created by KiberPerdun on 2/18/26.
//

#include "if_packet.h"

u64
sctp_push_to_xdp_segs (sctp_association_t *assoc, u64 len)
{
  if (assoc->xdp_seg_count >= XDP_MAX_SEGS)
    return -1;

  u64 offset = (u64)((u8 *)assoc->umem_hdrs_cursor - (u8 *)assoc->xdp->umem);

  assoc->xdp_segs[assoc->xdp_seg_count] = (u0 *) offset;
  assoc->xdp_seg_lens[assoc->xdp_seg_count] = len;

  ++assoc->xdp_seg_count;

  return 0;
}
