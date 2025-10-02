//
// Created by KiberPerdun on 11.12.2024.
//

#include "checks.h"
#include "eth.h"
#include "hwinfo.h"
#include "if_packet.h"
#include "netlink.h"
#include "random.h"
#include "types.h"

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <thread_db.h>
#include <threads.h>
#include <time.h>

#define MAX_PACKET_LEN 1516
u0 *
create_server ()
{
  /* init */
  connection_sctp_state_t *state = NULL;
  u0 *packet = NULL;
  thread_t cons, prod;
  ringbuf_t *rb_tx, *rb_rx;
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

  eth = eth_open (SERVER_INAME);
  rb_arg_t arg_tx;
  arg_tx.eth = eth;
  rb_tx = create_ringbuf (1024);
  arg_tx.rb = rb_tx;
  if (pthread_create (&cons, NULL, eth_send_rb, &arg_tx) != 0)
    return 0;

  rb_arg_t arg_rx;
  arg_rx.eth = eth;
  rb_rx = create_ringbuf (1024);
  arg_rx.rb = rb_rx;
  if (pthread_create (&prod, NULL, recv_packet, &arg_rx) != 0)
    return 0;

  /*
  pthread_t receiver;
  if (pthread_create (&receiver, NULL, recv_packet_to_ring_buffer, eth) != 0)
    return 0;
    */

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

  sctp_association_t *assoc;
  if (!((assoc = aligned_alloc (CACHELINE_SIZE, sizeof (sctp_association_t) + (CACHELINE_SIZE - 1) & ~(CACHELINE_SIZE - 1)))))
    goto cleanup;

  assoc->id = get_random_u16 ();
  pthread_spin_init (&assoc->lock, PTHREAD_PROCESS_PRIVATE);
  assoc->tx_ring = rb_tx;
  assoc->rx_ring = rb_rx;
  assoc->retry_ring = NULL;
  assoc->base = frame;
  assoc->base->state = meta;
  //assoc->ulp = ulp;

  sctp_init ();

  ringbuf_cell_t *cell;
  do
    for (; (cell = pop_ringbuf (rb_rx)) == 0;)
      ;

  while (if_ip_sctp (cell->packet, cell->plen, assoc) == 0);

  frame = build_sctp_init_ack_hdr (frame);
  push_ringbuf (rb_tx, frame->packet, frame->plen);
  frame->plen = 0;

  do
    for (; (cell = pop_ringbuf (rb_rx)) == 0;)
      ;

  while (if_ip_sctp (cell->packet, cell->plen, assoc) == 0);

  memcpy (frame->state, meta->add, meta->add_len);
  frame = build_sctp_cookie_ack_hdr (frame);
  push_ringbuf (rb_tx, frame->packet, frame->plen);
  frame->plen = 0;

  if (pthread_join (cons, NULL) != 0)
    return 0;

cleanup:
  eth_close (eth);
  free (state);
  free (meta);
  free (packet);
  free (frame);
}
#undef MAX_PACKET_LEN
