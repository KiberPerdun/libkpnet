//
// Created by KiberPerdun on 11.12.2024.
//

#include "checks.h"
#include "eth.h"
#include "hwinfo.h"
#include "if_packet.h"
#include "netlink.h"
#include "prefilled.h"
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
  ringbuf_t *rb_tx, *rb_rx, *rb_prefill;
  eth_t *eth;
  frame_data_t *frame;

  kpnet_env_t *env = kpnet_env_init (SERVER_INAME);
  if (!env) return NULL;

  u32 src_ip = inet_addr ("192.168.1.3");
  u16 src_port = 80;
  u32 dst_ip = ~0;
  u16 dst_port = ~0;

  sctp_association_t *assoc = sctp_assoc_create (
      env, src_ip, src_port, dst_ip, dst_port,
      SCTP_LISTEN, SERVER_INAME, CLIENT_INAME);

  if (!assoc)
    goto cleanup;

cleanup:
  eth_close (eth);
  free (state);
  free (packet);
  free (frame);
}
#undef MAX_PACKET_LEN
