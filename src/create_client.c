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
#include <threads.h>
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
  frame_data_t *frame_call;

  if (!(frame = malloc (sizeof (frame_data_t))))
    goto cleanup;

  if (!(packet = calloc (1, MAX_PACKET_LEN)))
    goto cleanup;

  frame->packet = packet;
  frame->plen = MAX_PACKET_LEN;
  frame->proto = PROTO_STACK_IP_TCP;

  eth = eth_open ("wlan0-virt");

  dst_port = htons (80);
  src_port = get_random_u16 ();

  volatile u32 src_ip = inet_addr ("192.168.1.2");
  // setup_bpf_filter (eth->fd, *proto_type, *proto_type);

  /* init end */

  /* FIN ACK 0x011 */

  switch (*proto_type)
    {
    case IPPROTO_TCP:
      {
        frame_sync_ip_tcp_t *frm_sync_tcpip
            = calloc (1, sizeof (frame_sync_ip_tcp_t));
        if (NULL == frm_sync_tcpip)
          return 0;

        if_ip_tcp_meta_t *meta = calloc (sizeof (if_ip_tcp_meta_t), 1);
        meta->state = TCP_SYN_SENT;
        meta->src_ip = src_ip;
        meta->src_port = src_port;

        frame->sync = frm_sync_tcpip;

        switch (setjmp (frame->jmpbuf))
          {
          case 0:
            {
              frame = fix_check_ip_tcp (
                  build_tcp_raw (
                      build_ip_raw (build_mac_raw (frame, "b2:eb:f1:b3:fb:88",
                                                   "wlan0-virt", ETHERTYPE_IP),
                                    src_ip, inet_addr ("192.168.1.3"),
                                    *proto_type, 40),
                      src_port, dst_port, meta->src_seq = get_random_u32 (), 0, 0x2, (u16)-1,
                      0, NULL),
                  MAX_PACKET_LEN);

              eth_send ((eth_t *)eth, (u0 *)packet, MAX_PACKET_LEN - frame->plen);

              recv_packet (eth->fd, if_ip_tcp, meta);

              frame->packet = packet;
              frame->plen = MAX_PACKET_LEN;

              frame = fix_check_ip_tcp (
    build_tcp_raw (
        build_ip_raw (build_mac_raw (frame, "46:9e:59:1e:5a:86",
                                     "wlan0-virt", ETHERTYPE_IP),
                      src_ip, inet_addr ("192.168.1.3"),
                      *proto_type, 40),
        src_port, dst_port, meta->src_seq = meta->src_seq + 1, meta->dst_seq + htonl (1), 0x10, (u16)-1,
        0, NULL),
    MAX_PACKET_LEN);

              eth_send ((eth_t *)eth, (u0 *)packet, MAX_PACKET_LEN - frame->plen);

              free (frm_sync_tcpip);
              free (meta);
              break;
            }
          case -1:
            return 0;
          default:;
          }

        break;
      }
    case PROTO_SCTP:
      {
        switch (setjmp (frame->jmpbuf))
          {
          case 0:
            {
              frame->sync = NULL;
              frame = build_sctp_init_hdr (
                  build_sctp_cmn_hdr_raw (
                      build_ip_raw (
                          build_mac_raw (frame, "46:9e:59:1e:5a:86",
                                                   "wlan0-virt", ETHERTYPE_IP),
                                    src_ip, inet_addr ("192.168.1.3"),
                                    *proto_type, 52),
                      src_port, dst_port, 0),
                  get_random_u32 (), -1, 32, 32, get_random_u32 ());

              frame
                  = fix_check_ip_sctp ((frame_data_t *)frame, MAX_PACKET_LEN);

              eth_send ((eth_t *)eth, (u0 *)packet,
                        MAX_PACKET_LEN - frame->plen);

              break;
            }
          default:break;
          }
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
