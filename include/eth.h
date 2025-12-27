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

typedef struct xdp_mmap_offsets xdp_mmap_offsets_t;

typedef struct xdp_socket
{
  i32 fd;
  u0 *umem;
  u64 umem_size;
  i32 ifindex;
  u32 queue_id;

  xdp_mmap_offsets_t offsets;
} xdp_t;

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

/* TODO!: Need testing on physical NIC */
xdp_t *xdp_open (const char *device, u32 queue_id);
xdp_t *xdp_close (xdp_t *xsk);
i32 xdp_send (xdp_t *xsk, u8 *buf, u64 len);
i32 xdp_recv (xdp_t *xsk, u8 *buf, u64 max_len);

#endif // LIBKPNET_ETH_H
