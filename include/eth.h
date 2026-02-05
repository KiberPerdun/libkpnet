//
// Created by KiberPerdun on 9/18/24.
//

#ifndef LIBKPNET_ETH_H
#define LIBKPNET_ETH_H

#include "linux/if_packet.h"
#include "ring_buffer.h"
#include "types.h"
#include <malloc.h>
#include <net/if.h>
#include <unistd.h>

#include <linux/if_xdp.h>
#include <linux/bpf.h>

#define FRAME_SIZE 2048
#define NUM_FRAMES 1024

typedef struct eth_handle
{
  i32 fd;
  struct ifreq ifr;
  struct sockaddr_ll sll;
} eth_t;

typedef struct
{
  ringbuf_t *rb;
  ringbuf_t *rb_prefill;
  ringbuf_t *allocator;
  eth_t *eth;
} rb_arg_t;

eth_t *eth_open (const char *device);
eth_t *eth_close (eth_t *eth);
ssize_t eth_send (eth_t *eth, u8 *buf, u64 len);
u0 *create_client ();
u0 *create_server ();

#endif // LIBKPNET_ETH_H
