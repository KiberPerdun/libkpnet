//
// Created by KiberPerdun on 07.01.26.
//

#ifndef XDP_H
#define XDP_H

#include "types.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/types.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

/* TODO!:
Удалить комментарий как только будет
реализована загрузка программы из кода
sudo bpftool prog loadall kp1.o /sys/fs/bpf/kp_net pinmaps /sys/fs/bpf/kp_net
sudo ls -l /sys/fs/bpf/kp_net/
sudo bpftool net attach xdp pinned /sys/fs/bpf/kp_net/kp_xdp_redirect dev libkpnet_c

sudo bpftool net detach xdp dev libkpnet_c
sudo rm -rf /sys/fs/bpf/kp_net
*/

/* Путь куда загружена map'а xdp программы */
#define MAP_PATH "/sys/fs/bpf/kp_net/xsks_map"

#include <linux/if_xdp.h>
typedef struct xdp_umem_reg xdp_umem_reg_t;
typedef struct xdp_ring_offset xdp_ring_offset_t;
typedef struct xdp_mmap_offsets xdp_mmap_offsets_t;
typedef struct xdp_desc xdp_desc_t;
typedef struct sockaddr_xdp sockaddr_xdp_t;

#define UMEM_CHUNK_SIZE 4096
#define UMEM_CHUNK_COUNT 4096
#define UMEM_LEN UMEM_CHUNK_SIZE * UMEM_CHUNK_COUNT
#define RING_MASK (UMEM_CHUNK_COUNT - 1)

typedef struct xdp_handle
{
  i32 fd;

  u0 *umem;

  u0 *rx_ring_mmap;
  u0 *tx_ring_mmap;
  u0 *fill_ring_mmap;
  u0 *completion_ring_mmap;

  u32 *rx_ring_consumer;
  u32 *rx_ring_producer;
  xdp_desc_t *rx_ring;

  u32 *tx_ring_consumer;
  u32 *tx_ring_producer;
  xdp_desc_t *tx_ring;

  u32 *fill_ring_consumer;
  u32 *fill_ring_producer;
  u64 *fill_ring;

  u32 *completion_ring_consumer;
  u32 *completion_ring_producer;
  u64 *completion_ring;

} xdp_t;

typedef struct umem_stack
{
  u64 *addrs;
  u32 top;
  u32 cap;
} umem_stack_t;

typedef struct umem_slab_allocator
{
  u0 *free_list;
  umem_stack_t *stack;
  u8 *cur_pos;
  u8 *cur_end;
} umem_slab_allocator_t;

/* Структура для Scatter-Gather передачи */
typedef struct xdp_sg_req
{
  u64 addr;
  u32 len;
  u32 flags;
} xdp_sg_req_t;

#define ring_smp_rmb() __asm__ __volatile__ ("" ::: "memory")
#define ring_smp_wmb() __asm__ __volatile__ ("" ::: "memory")

xdp_t *xdp_open (const char *device);
u32 xdp_rx (xdp_t *xsk, u64 *out, u32 *out_lens, u32 max_n);
u32 xdp_fill (xdp_t *xsk, umem_stack_t *stack, u32 max_n);
u32 xdp_tx (xdp_t *xsk, u0 **xdp_segs, const u32 *xdp_seg_lens, u32 n);
u32 xdp_complete_tx (xdp_t *xsk, u64 *out_addrs, u32 max_n);
u32 xdp_tx_sg (xdp_t *xsk, xdp_sg_req_t *reqs, u32 n);
u32 xdp_complete_tx (xdp_t *xsk, u64 *addr, u32 max_n);

umem_stack_t *create_umem_stack (u32 num_chunks, u32 chunk_size);
u64 umem_stack_pop (umem_stack_t *stack);
u0 umem_stack_push (umem_stack_t *stack, u64 addr);
u0 *slab_alloc_64b (umem_slab_allocator_t *alloc, u8 *umem_base);
u0 slab_free_64b (umem_slab_allocator_t *alloc, u0 *ptr);

#endif //XDP_H
