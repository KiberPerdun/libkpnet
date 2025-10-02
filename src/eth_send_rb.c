//
// Created by KiberPerdun on 9/26/25.
//

#include "if_packet.h"

_Noreturn u0 *
eth_send_rb (u0 *arg)
{
  rb_arg_t *a = arg;
  ringbuf_cell_t *cell;

  for (;;)
    {
      cell = pop_ringbuf (a->rb);
      if (cell)
        eth_send (a->eth, cell->packet, cell->plen);

      else
        sched_yield ();
    }
}
