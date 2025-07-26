//
// Created by KiberPerdun on 13.11.24.
//

#ifndef LIBKPNET_MAC_H
#define LIBKPNET_MAC_H

#include "if_packet.h"

typedef struct mac_hdr
{
  u8 mac[12];
  u16 type;
} mac_t;

bool build_mac_client_raw (const u8 *gateway, const u8 *self, u16 typelen, u0 *args);
mac_t *mac_client_fill_hdr (u16 typelen);
/* frame_data_t *build_mac_raw (frame_data_t *frame, const char *gateway, const char *dev, u16 typelen); */
u0 *build_mac_raw (u0 *packet, u16 *plen, const char *gateway, const char *dev, u16 typelen);

#endif // LIBKPNET_MAC_H
