#include "sctp.h"
#include <stdlib.h>

sctp_common_hdr_t *
sctp_fill_common_hdr (u16 src, u16 dst)
{
  sctp_common_hdr_t *hdr = malloc (sizeof (sctp_common_hdr_t));
  hdr->srcp = htons (src);
  hdr->dstp = htons (dst);
  hdr->tag = 0;
  hdr->checksum = 0;
  return hdr;
}
