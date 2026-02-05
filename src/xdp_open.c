//
// Created by KiberPerdun on 8/16/25.
//

#include "eth.h"
#include "netlink.h"
#include <netinet/in.h>
#include "xdp.h"

#define RING_DESC_SZ (UMEM_CHUNK_COUNT * sizeof (struct xdp_desc))
#define RING_ADDR_SZ (UMEM_CHUNK_COUNT * sizeof (u64))
xdp_t *
xdp_open (const char *device)
{
  xdp_mmap_offsets_t offsets;
  sockaddr_xdp_t sockaddr;
  xdp_umem_reg_t umem_reg;
  socklen_t offsets_len;
  static i32 ring_size;
  xdp_t *xdp;

  xdp = NULL;

  xdp = (xdp_t *) malloc (sizeof (xdp_t));
  if (!xdp)
      goto err_malloc;

  memset (xdp, 0, sizeof (xdp_t));
  xdp->fd = socket (AF_XDP, SOCK_RAW, 0);
  if (xdp->fd < 0)
      goto err_fd;

  xdp->umem = mmap (NULL, UMEM_LEN, PROT_READ | PROT_WRITE,MAP_PRIVATE
    | MAP_ANONYMOUS | MAP_POPULATE | MAP_LOCKED,-1, 0);
  if (xdp->umem == MAP_FAILED)
      goto err_umem_mmap;

  umem_reg.addr = (__u64) xdp->umem;
  umem_reg.len = UMEM_LEN;
  umem_reg.chunk_size = UMEM_CHUNK_SIZE;
  umem_reg.headroom = 0;
  umem_reg.flags = 0;

  if (setsockopt (xdp->fd, SOL_XDP, XDP_UMEM_REG, &umem_reg, sizeof (umem_reg)) < 0)
    goto err_umem_reg;

  ring_size = UMEM_CHUNK_COUNT;
  if (setsockopt (xdp->fd, SOL_XDP, XDP_RX_RING, &ring_size, sizeof (ring_size)) < 0)
    goto err_ring;

  if (setsockopt (xdp->fd, SOL_XDP, XDP_TX_RING, &ring_size, sizeof (ring_size)) < 0)
    goto err_ring;

  if (setsockopt (xdp->fd, SOL_XDP, XDP_UMEM_FILL_RING, &ring_size, sizeof (ring_size)) < 0)
    goto err_ring;

  if (setsockopt (xdp->fd, SOL_XDP, XDP_UMEM_COMPLETION_RING, &ring_size, sizeof (ring_size)) < 0)
    goto err_ring;

  memset (&offsets, 0, sizeof (offsets));
  offsets_len = sizeof (offsets);

  if (getsockopt (xdp->fd, SOL_XDP, XDP_MMAP_OFFSETS, &offsets, &offsets_len) < 0)
    goto err_xdp_offsets;

  xdp->rx_ring_mmap = mmap (
      NULL,
      offsets.rx.desc + RING_DESC_SZ,
      PROT_READ|PROT_WRITE,
      MAP_SHARED|MAP_POPULATE,
      xdp->fd,
      XDP_PGOFF_RX_RING);

  if (xdp->rx_ring_mmap == MAP_FAILED)
    goto err_rx_ring;

  xdp->tx_ring_mmap = mmap (
    NULL,
    offsets.tx.desc + RING_DESC_SZ,
    PROT_READ|PROT_WRITE,
    MAP_SHARED|MAP_POPULATE,
    xdp->fd,
    XDP_PGOFF_TX_RING);

  if (xdp->tx_ring_mmap == MAP_FAILED)
    goto err_tx_ring;

  xdp->fill_ring_mmap = mmap (
    NULL,
    offsets.fr.desc + RING_ADDR_SZ,
    PROT_READ|PROT_WRITE,
    MAP_SHARED|MAP_POPULATE,
    xdp->fd,
    XDP_UMEM_PGOFF_FILL_RING);

  if (xdp->fill_ring_mmap == MAP_FAILED)
    goto err_fr_ring;

  xdp->completion_ring_mmap = mmap (
      NULL,
      offsets.cr.desc + RING_ADDR_SZ,
      PROT_READ|PROT_WRITE,
      MAP_SHARED|MAP_POPULATE,
      xdp->fd,
      XDP_UMEM_PGOFF_COMPLETION_RING);

  if (xdp->completion_ring_mmap == MAP_FAILED)
    goto err_cr_ring;

  xdp->rx_ring_consumer = xdp->rx_ring_mmap + offsets.rx.consumer;
  xdp->rx_ring_producer = xdp->rx_ring_mmap + offsets.rx.producer;
  xdp->rx_ring = xdp->rx_ring_mmap + offsets.rx.desc;

  xdp->tx_ring_consumer = xdp->tx_ring_mmap + offsets.tx.consumer;
  xdp->tx_ring_producer = xdp->tx_ring_mmap + offsets.tx.producer;
  xdp->tx_ring = xdp->tx_ring_mmap + offsets.tx.desc;

  xdp->fill_ring_consumer = xdp->fill_ring_mmap + offsets.fr.consumer;
  xdp->fill_ring_producer = xdp->fill_ring_mmap + offsets.fr.producer;
  xdp->fill_ring = xdp->fill_ring_mmap + offsets.fr.desc;

  xdp->completion_ring_consumer = xdp->completion_ring_mmap + offsets.cr.consumer;
  xdp->completion_ring_producer = xdp->completion_ring_mmap + offsets.cr.producer;
  xdp->completion_ring = xdp->completion_ring_mmap + offsets.cr.desc;

  sockaddr.sxdp_family = AF_XDP;
  sockaddr.sxdp_flags = XDP_COPY;
  sockaddr.sxdp_ifindex = get_ifid (device);
  sockaddr.sxdp_queue_id = 0;
  sockaddr.sxdp_shared_umem_fd = xdp->fd;

  if (bind (xdp->fd, (struct sockaddr *) &sockaddr, sizeof (sockaddr_xdp_t)) < 0)
    goto err_bind;

  return xdp;
err_bind:
err_cr_ring:
  munmap (xdp->completion_ring_mmap, offsets.cr.desc + RING_ADDR_SZ);

err_fr_ring:
  munmap (xdp->fill_ring_mmap, offsets.fr.desc + RING_ADDR_SZ);

err_tx_ring:
  munmap (xdp->tx_ring_mmap, offsets.tx.desc + RING_DESC_SZ);

err_rx_ring:
  munmap (xdp->rx_ring_mmap, offsets.rx.desc + RING_DESC_SZ);

err_xdp_offsets:
err_ring:
err_umem_reg:
  munmap (xdp->umem, UMEM_LEN);

err_umem_mmap:
  close (xdp->fd);

err_fd:
err_malloc:
  free (xdp);

  return NULL;
}
#undef RING_DESC_SZ
#undef RING_ADDR_SZ
