//
// Created by KiberPerdun on 8/16/25.
//

#include "eth.h"
#include "netlink.h"

#include <netinet/in.h>

static i32
xdp_init_ring (struct xdp_ring_offset *ring, u0 *map, u32 entrs)
{
  ring->producer = (i64) map;
  ring->consumer = (i64) map + sizeof (u32);
  ring->desc = (i64) map + 2 * sizeof (u32);
  *(u32 *) ring->producer = 0;
  *(u32 *) ring->consumer = 0;

  return 0;
}

xdp_t *
xdp_open (const char *device, u32 queue_id)
{
  xdp_t *xsk = calloc (1, sizeof (xdp_t));

  if (xsk == NULL)
    return NULL;

  xsk->fd = socket (AF_XDP, SOCK_RAW, 0);
  if (xsk->fd < 0)
    {
      free (xsk);
      return NULL;
    }

  xsk->ifindex = get_ifid (device);
  xsk->queue_id = queue_id;

  xsk->umem_size = NUM_FRAMES * FRAME_SIZE;
  xsk->umem = mmap (NULL, xsk->umem_size, PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  if (xsk->umem == MAP_FAILED)
    {
      close (xsk->fd);
      free (xsk);
      return NULL;
    }

  struct xdp_umem_reg umem_reg =
    {
      .addr = (u64) xsk->umem,
      .len = xsk->umem_size,
      .chunk_size = FRAME_SIZE,
      .headroom = 0,
      .flags = 0
    };

  if (setsockopt (xsk->fd, SOL_XDP, XDP_UMEM_REG, &umem_reg, sizeof (umem_reg)))
    {
      perror ("Failed to register umem");
      munmap (xsk->umem, xsk->umem_size);
      close (xsk->fd);
      free (xsk);
      return NULL;
    }

  /* rings setup */
  socklen_t optlen = sizeof (xdp_mmap_offsets_t);

  if (getsockopt (xsk->fd, SOL_XDP, XDP_MMAP_OFFSETS, &xsk->offsets, &optlen) < 0)
    {
      perror ("Failed to get mmap offsets");
      munmap (xsk->umem, xsk->umem_size);
      close (xsk->fd);
      free (xsk);
      return NULL;
    }

  u32 ds = NUM_FRAMES / 2;
  if (setsockopt (xsk->fd, SOL_XDP, XDP_UMEM_COMPLETION_RING, &ds, sizeof (ds)))
    {
      perror ("Failed to config completion ring");
      munmap (xsk->umem, xsk->umem_size);
      close (xsk->fd);
      free (xsk);
      return NULL;
    }

  ds = NUM_FRAMES / 2;
  if (setsockopt (xsk->fd, SOL_XDP, XDP_UMEM_FILL_RING, &ds, sizeof (ds)))
    {
      perror ("Failed to config fill ring");
      munmap (xsk->umem, xsk->umem_size);
      close (xsk->fd);
      free (xsk);
      return NULL;
    }

  ds = NUM_FRAMES / 2;
  if (setsockopt (xsk->fd, SOL_XDP, XDP_TX_RING, &ds, sizeof (ds)))
    {
      perror ("Failed to config tx ring");
      munmap (xsk->umem, xsk->umem_size);
      close (xsk->fd);
      free (xsk);
      return NULL;
    }

  ds = NUM_FRAMES / 2;
  if (setsockopt (xsk->fd, SOL_XDP, XDP_RX_RING, &ds, sizeof (ds)))
    {
      perror ("Failed to config rx ring");
      munmap (xsk->umem, xsk->umem_size);
      close (xsk->fd);
      free (xsk);
      return NULL;
    }

  u0 *map_fr = mmap (NULL, xsk->offsets.fr.desc + (NUM_FRAMES / 2) * sizeof (u32), PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_POPULATE, xsk->fd, XDP_UMEM_PGOFF_FILL_RING);
  if (map_fr == MAP_FAILED)
    {
      perror ("Failed mmap fr");
      munmap (xsk->umem, xsk->umem_size);
      close (xsk->fd);
      free (xsk);
      return NULL;
    }
  xdp_init_ring (&xsk->offsets.fr, map_fr, NUM_FRAMES / 2);

  u0 *map_cr = mmap (NULL, xsk->offsets.cr.desc + (NUM_FRAMES / 2) * sizeof (u32), PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_POPULATE, xsk->fd, XDP_UMEM_PGOFF_COMPLETION_RING);
  if (map_cr == MAP_FAILED)
    {
      perror ("Failed mmap cr");
      munmap (xsk->umem, xsk->umem_size);
      close (xsk->fd);
      free (xsk);
      return NULL;
    }
  xdp_init_ring (&xsk->offsets.cr, map_cr, NUM_FRAMES / 2);

  u0 *map_rx = mmap (NULL, xsk->offsets.rx.desc + (NUM_FRAMES / 2) * sizeof (u32), PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_POPULATE, xsk->fd, XDP_PGOFF_RX_RING);
  if (map_rx == MAP_FAILED)
    {
      perror ("Failed mmap rx");
      munmap (xsk->umem, xsk->umem_size);
      close (xsk->fd);
      free (xsk);
      return NULL;
    }
  xdp_init_ring (&xsk->offsets.rx, map_rx, NUM_FRAMES / 2);

  u0 *map_tx = mmap (NULL, xsk->offsets.tx.desc + (NUM_FRAMES / 2) * sizeof (u32), PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_POPULATE, xsk->fd, XDP_PGOFF_TX_RING);
  if (map_tx == MAP_FAILED)
    {
      perror ("Failed mmap tx");
      munmap (xsk->umem, xsk->umem_size);
      close (xsk->fd);
      free (xsk);
      return NULL;
    }
  xdp_init_ring (&xsk->offsets.tx, map_tx, NUM_FRAMES / 2);

  u32 idx = 0;
  u64 *fill = (u64 *) xsk->offsets.fr.desc;
  for (i32 i = 0; i < NUM_FRAMES / 2; ++i)
    {
      *fill = idx * FRAME_SIZE;
      ++fill;
      ++idx;
    }

  *(u32 *) xsk->offsets.fr.producer = NUM_FRAMES / 2;

  struct sockaddr_xdp addr = {
    .sxdp_family = AF_XDP,
    .sxdp_ifindex = xsk->ifindex,
    .sxdp_queue_id = xsk->queue_id,
    .sxdp_flags = 0
  };

  if (bind (xsk->fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
      perror ("Failed to bind xdp socket");
      munmap (xsk->umem, xsk->umem_size);
      close (xsk->fd);
      free (xsk);
      return NULL;
    }

  return xsk;
}
