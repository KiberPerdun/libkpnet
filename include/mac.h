//
// Created by KiberPerdun on 13.11.24.
//

#ifndef LIBKPNET_MAC_H
#define LIBKPNET_MAC_H

#include "types.h"

typedef struct mac_hdr
{
  u8 mac[12];
  u16 type;
} mac_t;

mac_t *build_mac_client_raw (const u8 *gateway, const u8 *self, u16 typelen);
mac_t *mac_client_fill_hdr (u16 typelen);

#endif // LIBKPNET_MAC_H
