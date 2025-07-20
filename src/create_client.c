//
// Created by KiberPerdun on 11.12.2024.
//

#include "checks.h"
#include "eth.h"
#include "get_random.h"
#include "if_packet.h"
#include "ipv4.h"
#include "tcp.h"
#include "types.h"
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>

u0 *
create_client ()
{
  /* init */
  u0 *packet = NULL;
  eth_t *eth;
  u16 src_port, dst_port;
  frame_data_t *frame;

  if (!((frame = malloc (sizeof (frame_data_t)))))
    goto cleanup;

  if (!((packet = calloc (1, MAX_PACKET_LEN))))
    goto cleanup;

  frame->packet = packet;
  frame->plen = MAX_PACKET_LEN;

  eth = eth_open ("libkpnet_c");

  dst_port = htons (80);
  src_port = get_random_u16 ();

  volatile u32 src_ip = inet_addr ("192.168.1.2");

  frame->sync = NULL;
  frame = build_mac_raw (frame, "libkpnet_s", "libkpnet_c", ETHERTYPE_IP);
  frame = build_ip_raw (frame, src_ip, inet_addr ("192.168.1.3"), IPPROTO_SCTP, 52);
  frame = build_sctp_cmn_hdr_raw (frame, src_port, dst_port, 0);
  frame = build_sctp_init_hdr (frame, get_random_u32 (), -1, 32, 32, get_random_u32 ());
  frame = fix_check_ip_sctp (frame, MAX_PACKET_LEN);

  eth_send (eth, packet, MAX_PACKET_LEN - frame->plen);

cleanup:
  eth_close (eth);
  free (packet);
  free (frame);
}
