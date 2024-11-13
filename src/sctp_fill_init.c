#include "sctp.h"
#include <malloc.h>

sctp_init_t *
sctp_fill_init (u16 inbound_streams, u16 outbound_streams, u32 init, u32 tsn,
                u32 a_rwnd)
{
  sctp_init_t *hdr = malloc (sizeof (sctp_init_t));
  hdr->init_tag = htonl (init);
  hdr->a_rwnd = htonl (a_rwnd);
  hdr->iout = htons (inbound_streams);
  hdr->sout = htons (outbound_streams);
  hdr->tsn = htonl (tsn);
  return hdr;
}
