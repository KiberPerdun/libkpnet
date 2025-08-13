//
// Created by KiberPerdun on 3/14/25.
//

#include "get_random.h"
#include "if_packet.h"

static u_long romu_x, romu_y;

void romuduojr_seed(u_long seed)
{
  romu_x=seed^0xA5A5A5A5A5A5A5A5UL;
  romu_y=seed*0x5851F42D4C957F2DUL+1;
}
#define ROTL64(d,lrot) (((d)<<(lrot))|((d)>>(64-(lrot))))

u_long romuduojr(void)
{
  u_long xp=romu_x;
  romu_x=15241094284759029579u*romu_y;
  romu_y=romu_y-xp;
  romu_y=ROTL64(romu_y,27);
  return xp;
}

u_long randnum(u_long min, u_long max)
{
  if (min>max)
    return 0;
  if (min==max)
    return min;
  return min+(romuduojr()%(max-min+1UL));
}


frame_data_t *
build_sctp_init_hdr (frame_data_t *frame)
{
  if_ip_sctp_meta_t *meta;

  if (__builtin_expect (NULL == frame, 0) || __builtin_expect (NULL == frame->packet, 0))
    return NULL;

  meta = frame->state;
  romuduojr_seed (509);
  frame->packet += sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t) + sizeof (sctp_fld_hdr_t) + sizeof (sctp_init_hdr_t);
  frame->packet = build_sctp_init_hdr_raw (frame->packet, &frame->plen, romuduojr (), meta->src_arwnd, meta->src_os, meta->src_mis, romuduojr ());
  frame->packet = build_sctp_fld_hdr_raw (frame->packet, &frame->plen, SCTP_INIT, 0, frame->plen + sizeof (sctp_fld_hdr_t));
  frame->packet = build_sctp_cmn_hdr_raw (frame->packet, &frame->plen, meta->src_port, meta->dst_port, 0);
  frame->packet = build_ip_raw (frame->packet, &frame->plen, meta->src_ip, meta->dst_ip, PROTO_SCTP);
  frame->packet = build_mac_raw (frame->packet, &frame->plen, meta->gateway, meta->dev, ETHERTYPE_IP);

  return frame;
}
