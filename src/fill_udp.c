#include "udp.h"

udp_t *
fill_udp (u16 src, u16 dst, u16 plen)
{
  udp_t *udp = malloc (sizeof (udp_t));
  udp->srcp = htons (src);
  udp->dstp = htons (dst);
  udp->sum = htons (0x3a8d);
  udp->len = htons (plen);
  return udp;
}
