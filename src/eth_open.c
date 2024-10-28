#include "eth.h"

eth_t *
eth_open (const char *device)
{
  if (!device)
    return NULL;

  eth_t *eth;
  eth = calloc (1, sizeof (eth_t));
  if (!eth)
    return eth;

  if ((eth->fd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 1)
    return eth_close (eth);

  strlcpy (eth->ifr.ifr_name, device, sizeof (eth->ifr.ifr_name));
  if (ioctl (eth->fd, SIOCGIFINDEX, &eth->ifr) < 0)
    return eth_close (eth);

  memset (&eth->sll, 0, sizeof (eth->sll));
  eth->sll.sll_family = AF_PACKET;
  eth->sll.sll_protocol = htons (ETH_P_IP);
  eth->sll.sll_ifindex = eth->ifr.ifr_ifindex;

  return eth;
}
