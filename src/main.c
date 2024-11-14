#include "arp.h"
#include "eth.h"
#include "ipv4.h"
#include "sctp.h"
#include "types.h"
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tcp.h"
#include <unistd.h>
#include "if_packet.h"

i32
main (u0)
{
  u0 *packet;
  eth_t *e;
  u16 plen;

  mac_t *mac = build_mac_client_raw ("9c:a2:f4:a2:a9:3f", "50:28:4a:b4:bf:2c",
                                     ETHERTYPE_IP);
  plen = sizeof (mac_t);
  packet = calloc (1516, 1);

  ipv4_t *ip_hdr
      = fill_ipv4 (inet_addr ("192.168.0.108"), inet_addr ("64.233.162.102"));

  plen += sizeof (ipv4_t);

  memcpy (packet, mac, 14);
  memcpy (packet + 14, ip_hdr, 20);

  u0 *tcp_init_hdr = build_tcp_init_hdr (32345, 80, &plen, 576, ip_hdr);
  if (!tcp_init_hdr)
    goto cleanup;

  memcpy (packet + 34, tcp_init_hdr, sizeof (tcp_t) + sizeof (tcp_opt_mss_t));
  ((ipv4_t *)(packet + 14))->len = htons ( plen - 14);
  ((ipv4_t *)(packet + 14))->checksum = in_check ((u16 *) (packet + 14), sizeof (ipv4_t));

  e = eth_open ("wlan0");
  eth_send (e, packet, plen);
  plen = 34;

  packet_args_t *args = calloc (1, sizeof (packet_args_t));
  if (!args)
    goto cleanup;

  args->srcport = 32345;
  args->dstport = 80;
  args->proto = 0x06;

  args->tp_layer.tcp.TCP_STATUS = 1;
  args->net_layer.ipv4 = *ip_hdr;
  args->tp_layer.tcp._tcp = *((tcp_t *) tcp_init_hdr);

  recv_filtered (e->fd, if_ipv4_tcp, args);
  u0 *tcp_ack = build_tcp_ack_hdr (32345, 80, &plen, args);

  memcpy (packet + 34, tcp_ack, sizeof (tcp_t));
  ((ipv4_t *)(packet + 14))->len = htons ( plen - 14);
  ((ipv4_t *)(packet + 14))->checksum = 0;
  ((ipv4_t *)(packet + 14))->checksum = in_check ((u16 *) (packet + 14), sizeof (ipv4_t));

  eth_send (e, packet, plen);

  eth_close (e);

cleanup:
  free (tcp_ack);
  free (args);
  free (tcp_init_hdr);
  free (packet);
  free (ip_hdr);
  free (mac);
}
