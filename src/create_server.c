//
// Created by KiberPerdun on 11.12.2024.
//

#include "eth.h"
#include "eth.h"
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


u0 *
create_server (u16 *proto_type)
{
  /* init */
  connection_args_t *args;
  u0 *packet = NULL;
  eth_t *eth;
  u16 src_port, dst_port;

  eth = eth_open ("wlan0-virt");

  time_t t;
  srand ((u32) time (&t));

  if (!(args = calloc (1, sizeof (connection_args_t))))
    goto cleanup;

  if (!(packet = calloc (1, MAX_PACKET_LEN)))
    goto cleanup;

  dst_port = 12345;
  src_port = 12345; /* 443 + rand () % 65000 */

  args->srcport = htons (src_port);
  args->dstport = htons (dst_port);
  args->proto = IPPROTO_SCTP;
  args->packet = packet;
  args->eth = eth;
  args->plen = 0;
  args->SCTP_STATUS = SCTP_LISTEN;
  /* init end */

  build_mac_client_raw ("5a:50:12:f7:b0:f5", "46:9e:59:1e:5a:86",
                        ETHERTYPE_IP, args);

  /* FIN ACK 0x011 */

  fill_ipv4 (inet_addr ("192.168.1.3"), inet_addr ("192.168.1.2"), *proto_type, args);

  switch (*proto_type)
    {
    case (IPPROTO_SCTP):
      {
        args->tp_layer.sctp = packet + sizeof (mac_t) + sizeof (ipv4_t);

        args->tp_layer.sctp = packet + sizeof (mac_t) + sizeof (ipv4_t);
        build_sctp_hdr_raw (12345, 12345, rand (), SCTP_INIT, 16, 16, 368, 0,
                            args);
        args->net_layer.ipv4->len += htons (52);
        args->net_layer.ipv4->checksum = 0;
        args->net_layer.ipv4->checksum
            = in_check ((u16 *)(args->net_layer.ipv4), sizeof (ipv4_t));
        args->plen += 32;

        recv_filtered (args->eth->fd, if_ipv4_sctp, args);

        build_sctp_hdr_raw (12345, 12345, rand (), SCTP_INIT_ACK, 16, 16, 368, 0,
                            args);
        args->net_layer.ipv4->checksum = 0;
        args->net_layer.ipv4->checksum
            = in_check ((u16 *)(args->net_layer.ipv4), sizeof (ipv4_t));
        eth_send (args->eth, args->packet, args->plen);

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
  free (args);
  free (packet);
}

