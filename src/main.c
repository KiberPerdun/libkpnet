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

  /*
    memcpy (packet, gateway_mac_addres, 6);
    memcpy (packet + 6, self_mac_addres, 6);
    memcpy (packet + 12, &ppp, 2);
    */
  memcpy (packet, mac, 14);
  memcpy (packet + 14, ip_hdr, 20);

  u0 *tcp_init_hdr = build_tcp_init_hdr (12345, 80, &plen, 576, ip_hdr);
  if (!tcp_init_hdr)
    goto cleanup;

  memcpy (packet + 34, tcp_init_hdr, sizeof (tcp_t) + sizeof (tcp_opt_mss_t));

  e = eth_open ("wlan0");
  eth_send (e, packet, plen);
  eth_close (e);

cleanup:
  free (tcp_init_hdr);
  free (packet);
  free (ip_hdr);
  free (mac);
  // free (tcp_init_hdr);
}
