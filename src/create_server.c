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

u0 *
create_server ()
{
  /* init */
  u0 *packet = NULL;
  eth_t *eth;
  u16 src_port;
  frame_data_t *frame;

  if (!((frame = malloc (sizeof (frame_data_t)))))
    goto cleanup;

  if (!((packet = calloc (1, MAX_PACKET_LEN))))
    goto cleanup;

  frame->packet = packet;
  frame->plen = MAX_PACKET_LEN;
  frame->proto = PROTO_STACK_IP_TCP;

  eth = eth_open ("libkpnet_s");

  src_port = htons (80);

  u32 src_ip = inet_addr ("192.168.1.3");

  if_ip_sctp_meta_t *meta = calloc (sizeof (if_ip_sctp_meta_t), 1);
  meta->state = SCTP_LISTEN;
  meta->src_ip = src_ip;
  meta->src_port = src_port;

  recv_packet (eth->fd, if_ip_sctp, meta);
  printf ("\nskibidi\n%d\n", meta->state);

  frame->sync = NULL;
  frame = build_mac_raw (frame, "libkpnet_c", "libkpnet_s", ETHERTYPE_IP);
  frame = build_ip_raw (frame, src_ip, inet_addr ("192.168.1.2"), IPPROTO_SCTP, 60);
  frame = build_sctp_cmn_hdr_raw (frame, src_port, meta->dst_port, meta->src_ver_tag);
  frame = build_sctp_init_ack_hdr (frame, get_random_u32 (), -1, 32, 32, get_random_u32 (), meta);
  frame = fix_check_ip_sctp (frame, MAX_PACKET_LEN);

  eth_send (eth, packet, MAX_PACKET_LEN - frame->plen);

cleanup:
  eth_close (eth);
  free (meta);
  free (packet);
  free (frame);
}
