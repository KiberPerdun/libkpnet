//
// Created by KiberPerdun on 03.05.2026.
//

#include "if_packet.h"
#include "hwinfo.h"
#include "random.h"
#include "netlink.h"

sctp_association_t *
sctp_assoc_create (kpnet_env_t *env, u32 src_ip, u16 src_port, u32 dst_ip, u16 dst_port, u32 initial_state, const char *gw_ifname, const char *dev_ifname)
{
  sctp_association_t *assoc = malloc (sizeof (sctp_association_t)); /* хахахах */
  if (!assoc)
    return NULL;

  memset (assoc, 0, sizeof (sctp_association_t));

  sctp_ulp_config_t *ulp = slab_alloc_64b (&env->state_alloc, env->xdp->umem);
  if_ip_sctp_meta_t *meta = slab_alloc_64b (&env->state_alloc, env->xdp->umem);
  frame_data_t *frame = slab_alloc_64b (&env->state_alloc, env->xdp->umem);

  if (!ulp || !meta || !frame)
    return NULL;

  meta->state = initial_state;
  meta->src_ip = src_ip;
  meta->dst_ip = dst_ip;
  meta->src_port = src_port;
  meta->dst_port = dst_port;
  meta->src_arwnd = 65535;
  meta->src_os = 32;
  meta->src_mis = 32;

  if (gw_ifname && get_ifmac (gw_ifname, meta->gateway))
    return NULL;

  if (dev_ifname && get_ifmac (dev_ifname, meta->dev))
    return NULL;

  ulp->src_ip = src_ip;
  ulp->dst_ip = dst_ip;
  ulp->src_port = src_port;
  ulp->dst_port = dst_port;
  ulp->src_arwnd = 65535;
  ulp->src_os = 32;
  ulp->src_mis = 32;

  frame->state = meta;
  frame->sync = NULL;
  frame->plen = 0;

  memset (assoc->rtx, 0, sizeof (sctp_chunk_slot_t) * RTX_BUFFER_SIZE);
  assoc->allocator_64 = create_allocator (64, 256);
  assoc->bundling = create_ringbuf (256);
  assoc->id = get_random_u16 ();
  pthread_spin_init (&assoc->lock, PTHREAD_PROCESS_PRIVATE);

  assoc->ulp = ulp;
  assoc->base = frame;
  assoc->status = initial_state;
  assoc->os = htons (16);
  assoc->mis = htons (16);
  assoc->mtu = 1500 - sizeof (mac_t) - sizeof (ipv4_t) - sizeof (sctp_cmn_hdr_t);

  assoc->eth = env->eth;
  assoc->xdp = env->xdp;
  assoc->stack = env->stack;
  assoc->xdp_seg_count = 0;

  assoc->prefilled_umem_packet = slab_alloc_64b (&env->state_alloc, env->xdp->umem);
  if (gw_ifname && dev_ifname)
    prefill_sctp_mac_ip (assoc, *((u64 *) meta->gateway), *((u64 *) meta->dev));

  return assoc;
}
#undef ALIGN_UP
