#include "eth.h"
#include <stdlib.h>

mac_t *
get_gateway_mac_address (const char *ipp)
{
  mac_t *macc = malloc (sizeof (mac_t));
  FILE *fp = fopen("/proc/net/arp", "r");
  if (fp == NULL) {
      perror("Failed to open /proc/net/arp");
      exit (-1);
    }

  char line[256];
  fgets(line, sizeof(line), fp);

  while (fgets(line, sizeof(line), fp) != NULL) {
      char ip[32], hw_type[8], flags[8], mac[32], mask[32], device[32];
      sscanf(line, "%s %s %s %s %s %s", ip, hw_type, flags, mac, mask, device);
      u8 temp[8];
      if (sscanf (mac, "%x:%x:%x:%x:%x:%x", &temp[0], &temp[1], &temp[2], &temp[3],
                  &temp[4], &temp[5])
          != 6)
        exit (0);

      macc->addr = *((u64 *) temp) & 0xFFFFFFFFFFFF;

      if (strcmp (ipp, ip) == 0) {
          printf ("MAC address of gateway %s: %s\n", ip, mac);
          break;
        }
    }

  fclose(fp);
  return macc;
}
