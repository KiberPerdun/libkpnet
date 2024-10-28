#include "eth.h"

ssize_t
eth_send (eth_t *eth, u8 *buf, u64 len)
{
  if (!eth || !buf || len < sizeof (struct mac_hdr))
    return -1;

  struct mac_hdr *hdr;

  hdr = (struct mac_hdr *)buf;
  eth->sll.sll_protocol = hdr->type;

  return sendto (eth->fd, buf, len, 0, (const struct sockaddr *)&eth->sll,
                 sizeof (eth->sll));
}
