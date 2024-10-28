//
// Created by KiberPerdun on 9/18/24.
//

#ifndef LIBKPNET_ETH_H
#define LIBKPNET_ETH_H

#include "types.h"
#include <linux/if_ether.h>
#include <malloc.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netpacket/packet.h>
#include <stddef.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MAC_LEN 6
#define MAC_BUFFER_LEN (MAC_LEN * 3)

typedef struct mac_addr {
  u64 addr;
} mac_t;

typedef struct eth_handle
{
  i32 fd;
  struct ifreq ifr;
  struct sockaddr_ll sll;
} eth_t;

struct mac_hdr
{
  u8 macs[12];
  u16 type;
};

eth_t *eth_open (const char *device);
eth_t *eth_close (eth_t *eth);
ssize_t eth_send (eth_t *eth, u8 *buf, u64 len);
mac_t *get_mac_address (const char *device);
mac_t *get_gateway_mac_address (const char *ip);

#endif // LIBKPNET_ETH_H
