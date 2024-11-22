//
// Created by KiberPerdun on 13.11.24.
//

#include "mac.h"
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include "if_packet.h"

bool
build_mac_client_raw (const u8 *gateway, const u8 *self, u16 typelen, u0 *ars)
{
  connection_args_t *args;
  char octet[3];
  u8 octet_num;
  mac_t *mac;

  args = ars;

  if (args->packet == NULL)
    return false;

  if (!(MAX_PACKET_LEN >= sizeof (mac_t)))
    return false;

  if ((mac = malloc (sizeof (mac_t))) == NULL)
    {
      free (mac);
      return false;
    }

  octet_num = 0;

  for (int i = 0, j = 0; i < 6; ++i)
    {
      for (; gateway[j] != ':' && gateway[j] != '\0';)
        octet[octet_num++] = gateway[j++];

      octet[octet_num] = '\0';
      ((u8 *)mac)[i] = (u8) strtol (octet, NULL, 16);
      octet_num = 0;
      j++;
    }

  for (int i = 6, j = 0; i < 12; ++i)
    {
      for (; gateway[j] != ':' && gateway[j] != '\0';)
        octet[octet_num++] = self[j++];

      octet[octet_num] = '\0';
      ((u8 *)mac)[i] = (u8) strtol (octet, NULL, 16);
      octet_num = 0;
      j++;
    }

  mac->type = htons (typelen);
  memcpy (args->packet, mac, args->plen += sizeof (mac_t));

  free (mac);

  return true;
}
