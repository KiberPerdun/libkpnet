//
// Created by KiberPerdun on 9/18/24.
//

#ifndef LIBKPNET_ETH_H
#define LIBKPNET_ETH_H

#include "types.h"

#define MAC_LEN 6
#define MAC_BUFFER_LEN (MAC_LEN * 3)

typedef struct mac_addr {
  u64 addr;
} mac_t;

mac_t *get_self_mac();
mac_t *get_gateway_mac();

#endif // LIBKPNET_ETH_H
