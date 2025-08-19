//
// Created by KiberPerdun on 8/16/25.
//

#include "eth.h"

#include <errno.h>
#include <string.h>

i32
xdp_send (xdp_t *xsk, u8 *buf, u64 len)
{
  if (!xsk || !buf || len == 0 || len > FRAME_SIZE)
    {
      errno = EINVAL;
      return -1;
    }

  u32 *tx_producer = (u32 *)xsk->offsets.tx.producer;
  u32 *tx_consumer = (u32 *)xsk->offsets.tx.consumer;
  struct xdp_desc *tx_descs = (struct xdp_desc *)xsk->offsets.tx.desc;
  u32 ring_size = NUM_FRAMES / 2;

  u32 tx_idx = *tx_producer & (ring_size - 1);
  u32 free_slots = ring_size - (*tx_producer - *tx_consumer);

  if (free_slots == 0)
    {
      errno = EAGAIN;
      return -1;
    }

  u64 frame_addr = 0;
  u32 *cr_consumer = (u32 *)xsk->offsets.cr.consumer;
  u32 *cr_producer = (u32 *)xsk->offsets.cr.producer;

  if (*cr_producer != *cr_consumer)
    {
      u64 *cr_descs = (u64 *)xsk->offsets.cr.desc;
      frame_addr = cr_descs[*cr_consumer & (ring_size - 1)];
      (*cr_consumer)++;
    }
  else
    {
      u32 *fr_consumer = (u32 *)xsk->offsets.fr.consumer;
      u32 *fr_producer = (u32 *)xsk->offsets.fr.producer;

      if (*fr_producer == *fr_consumer)
        {
          fprintf (stderr, "No frames available for transmission\n");
          errno = ENOBUFS;
          return -1;
        }

      u64 *fr_descs = (u64 *)xsk->offsets.fr.desc;
      frame_addr = fr_descs[*fr_consumer & (ring_size - 1)];
      (*fr_consumer)++;
    }

  void *frame = xsk->umem + frame_addr;
  memcpy (frame, buf, len);

  tx_descs[tx_idx].addr = frame_addr;
  tx_descs[tx_idx].len = len;
  tx_descs[tx_idx].options = 0;

  __sync_synchronize ();

  (*tx_producer)++;

  int ret = 0;
  u32 flags = 0;
  if (recv (xsk->fd, &flags, sizeof (flags), MSG_DONTWAIT | MSG_PEEK) < 0)
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
          ret = write(xsk->fd, NULL, 0);
          if (ret < 0 && errno != EAGAIN && errno != EBUSY)
            {
              perror ("Failed to send packet");
              return -1;
            }
        }
      else
        {
          perror ("recv failed");
          return -1;
        }
    }

  return (ret < 0 && errno != EAGAIN && errno != EBUSY) ? -1 : 0;
}