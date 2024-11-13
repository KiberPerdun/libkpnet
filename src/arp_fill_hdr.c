#include "arp.h"
#include <malloc.h>
#include <netinet/in.h>

arp_t *
arp_fill_hdr (void)
{
  arp_t *hdr = malloc (sizeof (arp_t));
  hdr->proto = htons (ARP_HRD_ETHER);
  hdr->hrd = htons (ARP_PRO_IP);
  hdr->hln = ETH_ADDR_LEN;
  hdr->pln = IP_ADDR_LEN;
  hdr->op = htons (ARP_OP_REQUEST);
  return hdr;
}
