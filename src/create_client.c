//
// Created by KiberPerdun on 11.12.2024.
//

#include "checks.h"
#include "eth.h"
#include "if_packet.h"
#include "ipv4.h"
#include "netlink.h"
#include "random.h"
#include "ring_buffer.h"
#include "tcp.h"
#include "types.h"

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>

#define NS_PER_SEC 1000000000LL

#define BENCH_START() \
struct timespec _bench_start, _bench_end; \
clock_gettime(CLOCK_MONOTONIC, &_bench_start);

#define BENCH_END(name, ops) \
clock_gettime(CLOCK_MONOTONIC, &_bench_end); \
int64_t _ns = (_bench_end.tv_sec - _bench_start.tv_sec) * NS_PER_SEC + \
(_bench_end.tv_nsec - _bench_start.tv_nsec); \
printf("%s: %ld ns total, %.2f ns/op, %.2f Mops/sec\n", \
name, _ns, (double)_ns/(ops), ((double)ops*1e3)/_ns);

#define MAX_PACKET_LEN 1516
u0 *
create_client ()
{
  /* init */
  connection_sctp_state_t *state = NULL;
  u0 *packet = NULL, *tx_ring;
  eth_t *eth;
  u16 src_port, dst_port;
  frame_data_t *frame;

  if (!((frame = aligned_alloc (64, sizeof (frame_data_t) + 63 & ~63))))
    goto cleanup;

  if (!((packet = aligned_alloc (64, MAX_PACKET_LEN + 63 & ~63))))
    goto cleanup;

  if (!((state = aligned_alloc (64, sizeof (connection_sctp_state_t) + 63 & ~63))))
    goto cleanup;

  memset (packet, 0, MAX_PACKET_LEN);
  _mm_prefetch (frame, _MM_HINT_T0);
  _mm_prefetch (packet, _MM_HINT_T0);
  _mm_prefetch (state, _MM_HINT_T0);

  frame->packet = packet;
  frame->plen = MAX_PACKET_LEN;

  atomic_init (&state->packet_proccessing, false);
  atomic_init (&state->shutdown_requested, false);
  frame->state = state;

  eth = eth_open ("libkpnet_c");

  ring_buffer_t *rb = init_ring_buffer ();
  rb = fill_ring_buffer (rb, 2048);
  eth->rb = rb;

  pthread_t receiver;
  if (pthread_create (&receiver, NULL, recv_packet_to_ring_buffer, eth) != 0)
    return 0;

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

  if (get_ifmac (SERVER_INAME, meta->gateway))
    {
      fputs ("Failed to get gateway interface mac\n", stderr);
      goto cleanup;
    }

  if (get_ifmac (CLIENT_INAME, meta->dev))
    {
      fputs ("Failed to get dev interface mac\n", stderr);
      goto cleanup;
    }

  frame->sync = NULL;
  frame->plen = 0;

  generate_random_buffer ();

  sleep (1);

  BENCH_START ();
  frame = build_sctp_init_hdr (frame);
  BENCH_END ("build_sctp_init_hdr", 1);
  eth_send (eth, frame->packet, frame->plen);
  frame->plen = 0;

  if (!eth)
    goto cleanup;

  u0 *buffer = get_next_address_ring_buffer_consumer (rb);
  for (; !if_ip_sctp (buffer, 2048, meta);)
    buffer = get_next_address_ring_buffer_consumer (rb);

  // recv_packet (eth->fd, if_ip_sctp, meta);
  frame = build_sctp_cookie_echo_hdr (frame);
  eth_send (eth, frame->packet, frame->plen);
  frame->plen = 0;

  pthread_cancel (receiver);
  if (pthread_join (receiver, NULL) != 0)
    ;

  free_ring_buffer (rb);

cleanup:
  eth_close (eth);
  free (meta);
  free (state);
  free (packet);
  free (frame);
}
#undef MAX_PACKET_LEN
