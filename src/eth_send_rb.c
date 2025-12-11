//
// Created by KiberPerdun on 9/26/25.
//

#include "if_packet.h"

_Noreturn u0 *
eth_send_sctp (u0 *arg)
{
  rb_arg_t *a = arg;
  ringbuf_cell_t *cell;

  for (;;)
    {
      cell = pop_ringbuf (a->rb);
      if (cell)
        {
          sctp_cmn_hdr_t *cmn;
          cmn = cell->packet + sizeof (mac_t) + sizeof (ipv4_t);
          cmn->check = 0;
          cmn->check = ~(u32) generate_crc32c_on_crc32c ((const u8 *) cmn, cell->plen - sizeof (mac_t) - sizeof (ipv4_t ), 0xFFFFFFFF);

          if (eth_send (a->eth, cell->packet, cell->plen) < 0)
            continue;

          else
            push_ringbuf (a->rb_prefill, cell->packet, cell->plen);
        }

      else
        sched_yield ();
    }
}
