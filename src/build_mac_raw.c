//
// Created by KiberPerdun on 2/21/25.
//

#include "if_packet.h"

#include <stdlib.h>
#include "unistd.h"

u0 *
build_mac_raw (u0 *packet, u16 *plen, const char *gateway, const char *dev, u16 typelen)
{
  if (NULL == packet || NULL == plen || NULL == gateway || NULL == dev)
    return NULL;

  packet -= sizeof (mac_t);

  mac_t *mac;

  mac = packet;
  mac->type = htons (typelen);
  memcpy (mac, gateway, 6);
  memcpy ((u0 *)mac + 6, dev, 6);

  *plen += sizeof (mac_t);

  return packet;
}
