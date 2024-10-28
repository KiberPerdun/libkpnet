#include "eth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mac_t *
get_self_mac ()
{
  struct mac_addr *mac = malloc (MAC_LEN);
  char *buf = malloc (MAC_BUFFER_LEN);
  u8 temp[8];

  struct _IO_FILE *pipe = popen (
      "arp | grep '_gateway[ \\t]' | awk '{print $3}' | tail -n 1",
      "r"); // arp | grep '_gateway[ \t]' | awk '{print $3}' | tail -n 1
  if (!pipe)
    exit (0);

  fgets (buf, MAC_BUFFER_LEN, pipe);
  printf ("%s fasfsaf \n", buf);

  if (sscanf (buf, "%x:%x:%x:%x:%x:%x", &temp[0], &temp[1], &temp[2], &temp[3],
              &temp[4], &temp[5])
      != 6)
    exit (0);

  mac->addr = *((u64 *)temp) & 0xFFFFFFFFFFFF;
  // printf ("%llu", *((u64 *) temp) & 0xFFFFFFFFFF);
  // free(buf);
  return mac;
}
