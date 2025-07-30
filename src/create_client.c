//
// Created by KiberPerdun on 11.12.2024.
//

#include "checks.h"
#include "eth.h"
#include "get_random.h"
#include "if_packet.h"
#include "ipv4.h"
#include "netlink.h"
#include "tcp.h"
#include "types.h"

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>

#define MAX_PACKET_LEN 1516
u0 *
create_client ()
{
  /* init */
  connection_sctp_state_t *state = NULL;
  u0 *packet = NULL;
  eth_t *eth;
  u16 src_port, dst_port;
  frame_data_t *frame;

  if (!((frame = malloc (sizeof (frame_data_t)))))
    goto cleanup;

  if (!((packet = calloc (1, MAX_PACKET_LEN))))
    goto cleanup;

  if (!((state = malloc (sizeof (connection_sctp_state_t)))))
    goto cleanup;

  frame->packet = packet;
  frame->plen = MAX_PACKET_LEN;

  atomic_init (&state->packet_proccessing, false);
  atomic_init (&state->shutdown_requested, false);
  frame->state = state;

  eth = eth_open ("libkpnet_c");

  dst_port = 80;
  src_port = get_random_u16 ();

  u32 src_ip = inet_addr ("192.168.1.2");
  u32 dst_ip = inet_addr ("192.168.1.3");

  if_ip_sctp_meta_t *meta = calloc (sizeof (if_ip_sctp_meta_t), 1);
  frame->state = meta;
  meta->state = SCTP_INIT_SENT;
  meta->src_ip = src_ip;
  meta->dst_ip = dst_ip;
  meta->src_port = src_port;
  meta->dst_port = dst_port;
  meta->src_arwnd = ~0;
  meta->src_os = 32;
  meta->src_mis = 32;

  get_ifmac (CLIENT_INAME, meta->dev);
  get_ifmac (SERVER_INAME, meta->gateway);

  frame->sync = NULL;
  frame->plen = 0;

  frame = build_sctp_init_hdr (frame);
  eth_send (eth, frame->packet, frame->plen);
  frame->plen = 0;

  recv_packet (eth->fd, if_ip_sctp, meta);
  frame = build_sctp_cookie_echo_hdr (frame);
  eth_send (eth, frame->packet, frame->plen);
  frame->plen = 0;

  /*
  frame = build_mac_raw (frame, "libkpnet_s", "libkpnet_c", ETHERTYPE_IP);
  frame = build_ip_raw (frame, meta->src_ip, meta->dst_ip, IPPROTO_SCTP, 52);
  frame = build_sctp_cmn_hdr_raw (frame, meta->src_port, meta->dst_port, 0);
  frame = build_sctp_init_hdr (frame, get_random_u32 (), meta->src_arwnd, meta->src_os, meta->dst_os, get_random_u32 ());
  frame = fix_check_ip_sctp (frame, MAX_PACKET_LEN);

  eth_send (eth, packet, MAX_PACKET_LEN - frame->plen);

  recv_packet (eth->fd, if_ip_sctp, meta);

  frame->packet -= sizeof (sctp_cmn_hdr_t) + sizeof (sctp_init_hdr_t) + sizeof (sctp_fld_hdr_t);
  frame->plen += sizeof (sctp_cmn_hdr_t) + sizeof (sctp_init_hdr_t)  + sizeof (sctp_fld_hdr_t);

  frame = build_sctp_cmn_hdr_raw (frame, meta->src_port, meta->dst_port, meta->src_ver_tag);
  frame = build_sctp_cookie_echo_hdr (frame, meta);
  frame = fix_check_ip_sctp (frame, MAX_PACKET_LEN);

  eth_send (eth, packet, MAX_PACKET_LEN - frame->plen);

  free (meta->add);
  */

cleanup:
  eth_close (eth);
  free (meta);
  free (state);
  free (packet);
  free (frame);
}
#undef MAX_PACKET_LEN
