#include "arp.h"
#include "eth.h"
#include "malloc.h"
#include "stdlib.h"
#include <arpa/inet.h>

arp_eth_t *
arp_fill_eth (void)
{
  return NULL;
  /*
  arp_eth_t *hdr = malloc (sizeof (arp_eth_t));
  hdr->sha = *get_mac_address ("wlan0");
  mac_t *m = calloc (6, 1);
  char *buf = malloc (MAC_BUFFER_LEN);
  u8 temp[MAC_LEN];

  struct _IO_FILE *pipe
      = popen ("ifconfig | grep wg1 -A 1 | tail -n 1 | awk '{print $2}'", "r");
  if (!pipe)
    exit (0);

  fgets (buf, 14, pipe);
  puts (buf);

  u32 ip;
  inet_pton (AF_INET, "192.168.0.103", &ip);
  hdr->spa = ip;
  inet_pton (AF_INET, "192.168.0.1", &ip);
  hdr->tpa = ip;
  hdr->tha = *m;
  free (m);
  free (buf);
  return hdr;
   */
}
