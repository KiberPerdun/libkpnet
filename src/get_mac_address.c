#include "eth.h"
#include <stdlib.h>

mac_t *
get_mac_address (const char *interface)
{
  struct mac_addr *mac = malloc (MAC_LEN);
  u8 temp[8];
  char path[256];
  snprintf (path, sizeof (path), "/sys/class/net/%s/address", interface);

  FILE *fp = fopen (path, "r");
  if (fp == NULL)
    {
      perror ("Failed to open MAC address file");
      exit (-1);
    }

  char mac_address[18];
  if (fgets (mac_address, sizeof (mac_address), fp) != NULL)
    {
      printf ("MAC address of %s: %s", interface, mac_address);
    }

  if (sscanf (mac_address, "%x:%x:%x:%x:%x:%x", &temp[0], &temp[1], &temp[2], &temp[3],
              &temp[4], &temp[5])
      != 6)
    exit (0);

  mac->addr = *((u64 *) temp) & 0xFFFFFFFFFFFF;

  fclose (fp);
  return mac;
}
