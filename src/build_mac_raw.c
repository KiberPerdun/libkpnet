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

  struct ifreq ifr = {0};
  mac_t *mac;
  i32 fd;


  mac = packet;

  mac->type = htons (typelen);
  fd = socket (AF_INET, SOCK_DGRAM, 0);

  snprintf (ifr.ifr_name, IFNAMSIZ, gateway);

  ioctl (fd, SIOCGIFHWADDR, &ifr);
  memcpy (mac, ifr.ifr_hwaddr.sa_data, 6);

  snprintf (ifr.ifr_name, IFNAMSIZ, dev);

  ioctl (fd, SIOCGIFHWADDR, &ifr);
  memcpy ((u0 *)mac + 6, ifr.ifr_hwaddr.sa_data, 6);

  close (fd);

  *plen += sizeof (mac_t);

  return packet;
}
