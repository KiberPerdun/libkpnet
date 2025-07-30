//
// Created by KiberPerdun on 11.12.2024.
//

#include "checks.h"
#include "eth.h"
#include "get_random.h"
#include "if_packet.h"
#include "types.h"
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>

#define MAX_PACKET_LEN 1516
u0 *
create_server ()
{
  /* init */
  connection_sctp_state_t *state = NULL;
  u0 *packet = NULL;
  eth_t *eth;
  u16 src_port;
  frame_data_t *frame;

  if (!((frame = malloc (sizeof (frame_data_t)))))
    goto cleanup;

  if (!((packet = calloc (1, MAX_PACKET_LEN))))
    goto cleanup;

  if (!((state = malloc (sizeof (connection_sctp_state_t)))))
    goto cleanup;

  atomic_init (&state->packet_proccessing, false);
  atomic_init (&state->shutdown_requested, false);
  frame->state = state;

  frame->packet = packet;
  frame->plen = 0;
  frame->proto = PROTO_STACK_IP_TCP;

  eth = eth_open ("libkpnet_s");

  src_port = 80;

  u32 src_ip = inet_addr ("192.168.1.3");

  if_ip_sctp_meta_t *meta = calloc (sizeof (if_ip_sctp_meta_t), 1);
  frame->state = meta;
  meta->state = SCTP_LISTEN;
  meta->src_ip = src_ip;
  meta->src_port = src_port;
  meta->src_arwnd = ~0;
  meta->src_os = 32;
  meta->src_mis = 32;

  recv_packet (eth->fd, if_ip_sctp, meta);
  frame = build_sctp_init_ack_hdr (frame);
  eth_send (eth, frame->packet, frame->plen);
  frame->plen = 0;

  recv_packet (eth->fd, if_ip_sctp, meta);
  frame->state = meta->add;
  frame = build_sctp_cookie_ack_hdr (frame);
  eth_send (eth, frame->packet, frame->plen);
  frame->plen = 0;

  /*

  frame->sync = NULL;
  frame = build_mac_raw (frame, "libkpnet_c", "libkpnet_s", ETHERTYPE_IP);
  frame = build_ip_raw (frame, meta->src_ip, meta->dst_ip, IPPROTO_SCTP, 60);
  frame = build_sctp_cmn_hdr_raw (frame, meta->src_port, meta->dst_port,
  meta->src_ver_tag); frame = build_sctp_init_ack_hdr (frame, meta->src_arwnd,
  meta->src_os,  meta->src_mis, meta); frame = fix_check_ip_sctp (frame,
  MAX_PACKET_LEN);

  eth_send (eth, packet, MAX_PACKET_LEN - frame->plen);
  */

cleanup:
  eth_close (eth);
  free (frame->state);
  free (state);
  free (meta);
  free (packet);
  free (frame);
}
#undef MAX_PACKET_LEN
