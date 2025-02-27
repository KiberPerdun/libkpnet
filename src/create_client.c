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
#include <string.h>
#include <time.h>

#include <linux/if_tun.h>

u0 *
create_client (u16 *proto_type)
{
  /* init */
  u0 *packet = NULL;
  eth_t *eth;
  u16 src_port, dst_port;
  frame_data_t *frame;

  if (!(frame = malloc (sizeof (frame_data_t))))
    goto cleanup;

  if (!(packet = calloc (1, MAX_PACKET_LEN)))
    goto cleanup;

  frame->packet = packet;
  frame->plen = MAX_PACKET_LEN;
  frame->proto = PROTO_STACK_IP_TCP;

  eth = eth_open ("wlan0-peer");

  dst_port = htons (80);
  src_port = get_random_u16 ();

  /* init end */

  /* FIN ACK 0x011 */

  switch (*proto_type)
    {
    case (IPPROTO_TCP):
      {
        frame_sync_ip_tcp_t *frm_sync_tcpip = calloc (1, sizeof (frame_sync_ip_tcp_t));
        if (NULL == frm_sync_tcpip)
          return 0;

        frame->sync = frm_sync_tcpip;

        frame =
          fix_check_ip_tcp (
          build_tcp_raw (
            build_ip_raw (
                build_mac_raw (frame,
                "46:9e:59:1e:5a:86", "5a:50:12:f7:b0:f5", ETHERTYPE_IP),
                        inet_addr ("192.168.1.2"), inet_addr ("192.168.1.3"), *proto_type),
                          src_port, dst_port, get_random_u32 (), 0, 0x2, (u16)-1, 0, NULL), MAX_PACKET_LEN);

        eth_send (eth, packet, MAX_PACKET_LEN - frame->plen);


        free (frm_sync_tcpip);
        break;
      }
    default:
      {
        puts ("Undefined proto type");
        goto cleanup;
      }
    }

cleanup:
  eth_close (eth);
  free (packet);
  free (frame);
}

