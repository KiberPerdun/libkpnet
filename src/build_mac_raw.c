//
// Created by KiberPerdun on 2/21/25.
//

#include "if_packet.h"

#include <stdlib.h>
#include "unistd.h"

frame_data_t *build_mac_raw
(frame_data_t *frame, const char *gateway, const char *dev, u16 typelen)
{
  if (frame->plen < sizeof (mac_t) | NULL == frame->packet)
    {
      frame->plen = 0;
      return frame;
    }

  struct ifreq ifr = {0};
  char octet[3];
  u8 octet_num;
  mac_t *mac;
  i32 fd;

  mac = frame->packet;
  mac->type = htons (typelen);
  fd = socket (AF_INET, SOCK_DGRAM, 0);

  snprintf (ifr.ifr_name, IFNAMSIZ, gateway);

  ioctl (fd, SIOCGIFHWADDR, &ifr);
  memcpy (mac, ifr.ifr_hwaddr.sa_data, 6);

  snprintf (ifr.ifr_name, IFNAMSIZ, dev);

  ioctl (fd, SIOCGIFHWADDR, &ifr);
  memcpy ((u0 *)mac + 6, ifr.ifr_hwaddr.sa_data, 6);

  close (fd);

  frame->packet += sizeof (mac_t);
  frame->plen -= sizeof (mac_t);

  return frame;
}
