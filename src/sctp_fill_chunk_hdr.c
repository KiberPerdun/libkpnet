#include "malloc.h"
#include "sctp.h"

sctp_chunk_hdr_t *
sctp_fill_chunk_hdr (u8 type, u8 flags, u16 plen)
{
  sctp_chunk_hdr_t *hdr = malloc (sizeof (sctp_chunk_hdr_t));
  hdr->type = type;
  hdr->flags = flags;
  hdr->len = htons (plen);
  return hdr;
}
