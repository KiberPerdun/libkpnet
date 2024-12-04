#include "eth.h"

size_t
_strlcpy(dst, src, siz)
char *dst;
const char *src;
size_t siz;
{
  register char *d = dst;
  register const char *s = src;
  register size_t n = siz;

  if (n != 0 && --n != 0) {
      do {
          if ((*d++ = *s++) == 0)
            break;
        } while (--n != 0);
    }

  if (n == 0) {
      if (siz != 0)
        *d = '\0';
      while (*s++)
        ;
    }

  return(s - src - 1);
}

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

  _strlcpy (eth->ifr.ifr_name, device, sizeof (eth->ifr.ifr_name));
  if (ioctl (eth->fd, SIOCGIFINDEX, &eth->ifr) < 0)
    return eth_close (eth);

  memset (&eth->sll, 0, sizeof (eth->sll));
  eth->sll.sll_family = AF_PACKET;
  eth->sll.sll_protocol = htons (ETH_P_IP);
  eth->sll.sll_ifindex = eth->ifr.ifr_ifindex;

  return eth;
}
