//
// Created by KiberPerdun on 10/9/25.
//

#include "if_packet.h"
#include "hwinfo.h"
#include "random.h"

i64
prepare_sctp_threads (sctp_association_t *assoc)
{
  sctp_thread_t **threads;

  if (!((threads = aligned_alloc (CACHELINE_SIZE, (sizeof (sctp_thread_t *) * (ntohs (assoc->mis) + ntohs (assoc->os))) + (CACHELINE_SIZE - 1) & ~(CACHELINE_SIZE - 1)))))
    {
      free (threads);
      return -1;
    }

  for (i64 i = 0; i < (ntohs (assoc->mis) + ntohs (assoc->os)); ++i)
    {
      if (!((threads[i] = aligned_alloc (CACHELINE_SIZE, sizeof (sctp_thread_t) + (CACHELINE_SIZE - 1) & ~(CACHELINE_SIZE - 1)))))
        {
          for (i64 j = 0; j < i; ++j)
            free (threads[j]);

          free (threads);
          return -1;
        }

      if (assoc->os >= i)
        threads[i]->id = i;

      else
        threads[i]->id = i - assoc->os;

      memset (threads[i], 0, sizeof (sctp_thread_t));
    }

  pthread_spin_lock (&assoc->lock);
  assoc->mis_threads = threads;
  assoc->os_threads = threads + ntohs (assoc->mis);
  pthread_spin_unlock (&assoc->lock);

  return 0;
}
