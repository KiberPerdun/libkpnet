#include "eth.h"

eth_t *
eth_close (eth_t *eth)
{
  if (eth)
    {
      if (eth->fd >= 0)
        close (eth->fd);
      free (eth);
    }
  return NULL;
}
