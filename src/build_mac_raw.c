//
// Created by KiberPerdun on 2/21/25.
//

#include "if_packet.h"

#include <stdlib.h>
#include "unistd.h"

frame_data_t *build_mac_raw
(volatile frame_data_t *frame, u0 *gateway, const char *dev, u16 typelen)
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

  octet_num = 0;
  mac = frame->packet;

  for (i32 i = 0, j = 0; i < 6; ++i)
    {
      while (((u8 *)gateway)[j] != ':' && ((u8 *)gateway)[j] != '\0')
        octet[octet_num++] = ((u8 *)gateway)[j++];

      octet[octet_num] = '\0';
      ((u8 *)mac)[i] = (u8) strtol (octet, NULL, 16);
      octet_num = 0;
      j++;
    }

  snprintf (ifr.ifr_name, IFNAMSIZ, dev);

  fd = socket (AF_INET, SOCK_DGRAM, 0);
  ioctl (fd, SIOCGIFHWADDR, &ifr);

  mac->type = htons (typelen);
  memcpy ((u0 *)mac + 6, ifr.ifr_hwaddr.sa_data, 6);
  close (fd);

  frame->packet += sizeof (mac_t);
  frame->plen -= sizeof (mac_t);

  return frame;
}
