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

int
main (void)
{
  mac_t *gateway_mac_addres, *self_mac_addres;
  char *src_ip, *dst_ip, *dev;
  void *packet;
  u16 plen;

  packet = calloc (1516, 1);
  src_ip = "172.20.10.8";
  dst_ip = "74.125.131.100"; // 64.233.162.139
  dev = "wlan0";
  plen = 0;

  gateway_mac_addres = get_gateway_mac_address (src_ip);
  self_mac_addres = get_mac_address (dev);

  u16 ppp = htons (ETHERTYPE_IP);
  ipv4_t *ip_hdr = fill_ipv4 (inet_addr (src_ip), inet_addr (dst_ip));

  memcpy (packet, gateway_mac_addres, 6);
  memcpy (packet + 6, self_mac_addres, 6);
  memcpy (packet + 12, &ppp, 2);
  memcpy (packet + 14, ip_hdr, 20);

  void *tcp_init_hdr = build_tcp_init_hdr (123, 80, &plen, 576, ip_hdr);
  memcpy (packet + 34, tcp_init_hdr, plen);

  eth_t *e;
  e = eth_open (dev);
  printf ("\n%hu\n", ntohs (e->sll.sll_protocol));
  e->sll.sll_protocol = 0x0800;
  eth_send (e, packet, 58);

  free (packet);
  free (gateway_mac_addres);
  free (self_mac_addres);
  free (ip_hdr);
}
