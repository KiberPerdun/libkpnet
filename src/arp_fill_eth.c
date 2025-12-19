#include "arp.h"
#include "eth.h"
#include "malloc.h"
#include "netlink.h"
#include "stdlib.h"
#include <arpa/inet.h>

arp_eth_t *
arp_fill_eth (void)
{
  arp_eth_t *hdr = malloc (sizeof (arp_eth_t));
  get_ifmac (CLIENT_INAME, (u0 *) &hdr->sha);
  //hdr->sha = *get_mac_address ("wlan0");

  u32 ip;
  hdr->spa = inet_addr ("192.168.1.2");
  hdr->tpa = inet_addr ("192.168.1.3");
  memset (&hdr->tha, 0 , 6);
  return hdr;
}
