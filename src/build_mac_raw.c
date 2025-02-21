//
// Created by KiberPerdun on 2/21/25.
//

#include "if_packet.h"

#include <stdlib.h>

frame_data_t *build_mac_raw
(frame_data_t *frame, u0 *gateway, u0 *self, u16 typelen)
{
  if (frame->plen < sizeof (mac_t) | NULL == frame->packet)
    {
      frame->plen = 0;
      return frame;
    }

  char octet[3];
  u8 octet_num;
  mac_t *mac;

  octet_num = 0;
  mac = frame->packet;

  for (int i = 0, j = 0; i < 6; ++i)
    {
      for (; ((u8 *)gateway)[j] != ':' && ((u8 *)gateway)[j] != '\0';)
        octet[octet_num++] = ((u8 *)gateway)[j++];

      octet[octet_num] = '\0';
      ((u8 *)mac)[i] = (u8) strtol (octet, NULL, 16);
      octet_num = 0;
      j++;
    }

  for (int i = 6, j = 0; i < 12; ++i)
    {
      for ( ; ((u8 *)gateway)[j] != ':' && ((u8 *)gateway)[j] != '\0' ; )
        octet[octet_num++] = ((u8 *)self)[j++];

      octet[octet_num] = '\0';
      ((u8 *)mac)[i] = (u8) strtol (octet, NULL, 16);
      octet_num = 0;
      j++;
    }

  mac->type = htons (typelen);

  frame->packet += sizeof (mac_t);
  frame->plen -= sizeof (mac_t);

  return frame;
}
