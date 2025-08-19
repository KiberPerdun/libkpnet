//
// Created by KiberPerdun on 11.12.2024.
//

#include "checks.h"
#include "eth.h"
#include "if_packet.h"
#include "netlink.h"
#include "random.h"
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

  ring_buffer_t *rb = init_ring_buffer ();
  rb = fill_ring_buffer (rb, 2048);
  eth->rb = rb;

  pthread_t receiver;
  if (pthread_create (&receiver, NULL, recv_packet_to_ring_buffer, eth) != 0)
    return 0;


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

  if (get_ifmac (SERVER_INAME, meta->dev))
    {
      fputs ("Failed to get dev interface mac\n", stderr);
      goto cleanup;
    }

  if (get_ifmac (CLIENT_INAME, meta->gateway))
    {
      fputs ("Failed to get gateway interface mac\n", stderr);
      goto cleanup;
    }

  if (!eth)
    goto cleanup;

  sleep (1);

  u0 *buffer = get_next_address_ring_buffer_consumer (rb);
  for (; !if_ip_sctp (buffer, 2048, meta);)
    buffer = get_next_address_ring_buffer_consumer (rb);

  //recv_packet (eth->fd, if_ip_sctp, meta);
  frame = build_sctp_init_ack_hdr (frame);
  eth_send (eth, frame->packet, frame->plen);
  frame->plen = 0;

  buffer = get_next_address_ring_buffer_consumer (rb);
  for (; !if_ip_sctp (buffer, 2048, meta);)
    buffer = get_next_address_ring_buffer_consumer (rb);

  //recv_packet (eth->fd, if_ip_sctp, meta);
  memcpy (frame->state, meta->add, meta->add_len);
  frame = build_sctp_cookie_ack_hdr (frame);
  eth_send (eth, frame->packet, frame->plen);
  frame->plen = 0;

  pthread_cancel (receiver);
  if (pthread_join (receiver, NULL) != 0)
    ;

  free_ring_buffer (rb);

cleanup:
  eth_close (eth);
  free (state);
  free (meta);
  free (packet);
  free (frame);
}
#undef MAX_PACKET_LEN
