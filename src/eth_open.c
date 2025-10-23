#include "if_packet.h"

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

/* TODO: AF_XDP socket */
eth_t *
eth_open (const char *device)
{
  i32 val = 1;

  if (!device)
    return NULL;

  eth_t *eth;
  eth = calloc (1, sizeof (eth_t));
  if (!eth)
    return eth;

  if ((eth->fd = socket (AF_PACKET, SOCK_RAW, htons (ETH_P_IP))) < 1)
    return eth_close (eth);

  _strlcpy (eth->ifr.ifr_name, device, sizeof (eth->ifr.ifr_name));
  if (ioctl (eth->fd, SIOCGIFINDEX, &eth->ifr) < 0)
    return eth_close (eth);

  memset (&eth->sll, 0, sizeof (eth->sll));
  eth->sll.sll_family = AF_PACKET;
  eth->sll.sll_protocol = htons (ETH_P_IP);
  eth->sll.sll_ifindex = eth->ifr.ifr_ifindex;
  eth->sll.sll_hatype = 1;
  eth->sll.sll_halen = ETH_ALEN;
  eth->sll.sll_pkttype = PACKET_HOST; /* TODO: pkttype */

  if (bind (eth->fd, (struct sockaddr *) &eth->sll, sizeof (eth->sll)) < 0)
    {
      perror ("bind");
      return eth_close (eth);
    }

  if (setsockopt (eth->fd, SOL_PACKET, PACKET_IGNORE_OUTGOING, &val,
                  sizeof (val))
      < 0)
      perror ("setsockopt PACKET_IGNORE_OUTGOING");

  i32 flags = fcntl (eth->fd, F_GETFL, 0);
  if (flags < 0)
    {
      perror ("fcntl F_GETFL");
      return eth_close (eth);
    }

  if (fcntl (eth->fd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
      perror ("fcntl F_SETFL O_NONBLOCK");
      return eth_close (eth);
    }

  return eth;
}
