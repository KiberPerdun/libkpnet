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

i32
main (u0)
{
  packet_args_t *args;
  u0 *packet;
  eth_t *e;
  u16 src_port, dst_port;
  mac_t *mac;

  time_t t;
  srand ((u32) time (&t));

  if (!(args = calloc (1, sizeof (packet_args_t))))
    goto cleanup;

  if (!(packet = calloc (1, 1500)))
    goto cleanup;

  dst_port = 80;
  src_port = 443 + rand () % 65000;

  args->srcport = src_port;
  args->dstport = dst_port;
  args->proto = IPPROTO_TCP;
  args->packet_len = 1500;
  args->packet = packet;

  mac = build_mac_client_raw ("9a:60:ca:f3:df:64", "50:28:4a:b4:bf:2c",
                               ETHERTYPE_IP);

  memcpy (packet, mac, sizeof (mac_t));
  args->plen = sizeof (mac_t);

  ipv4_t *ip_hdr
      = fill_ipv4 (inet_addr ("172.20.10.8"), inet_addr ("74.125.131.101"));

  memcpy (packet + args->plen, ip_hdr, sizeof (ipv4_t));
  args->plen += sizeof (ipv4_t);  /* 34 */
  args->net_layer.ipv4 = *ip_hdr;

  e = eth_open ("wlan0");
  tcp_make_handshake (args, e);
  eth_close (e);

cleanup:
  free (args);
  free (packet);
  free (ip_hdr);
  free (mac);
}
