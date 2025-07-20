//
// Created by KiberPerdun on 9/18/24.
//

#ifndef LIBKPNET_ETH_H
#define LIBKPNET_ETH_H

#include "types.h"
#include <malloc.h>
#include <net/if.h>
#include <unistd.h>
#include <netpacket/packet.h>

typedef struct eth_handle
{
  i32 fd;
  struct ifreq ifr;
  struct sockaddr_ll sll;
} eth_t;

eth_t *eth_open (const char *device);
eth_t *eth_close (eth_t *eth);
ssize_t eth_send (eth_t *eth, u8 *buf, u64 len);
u0 *create_client ();
u0 *create_server ();
u0  setup_bpf_filter (i32 fd, u32 ip, u16 type);

#endif // LIBKPNET_ETH_H
