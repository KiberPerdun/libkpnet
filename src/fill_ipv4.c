#include "ipv4.h"
#include "malloc.h"
#include <netinet/in.h>
#include <stdlib.h>
#include "if_packet.h"

bool
fill_ipv4 (u32 src, u32 dst, u8 proto, u0 *ars)
{
  connection_args_t *args;
  ipv4_t *hdr;

  args = ars;

  if (args->packet == NULL)
    return false;

  if (MAX_PACKET_LEN < args->plen + sizeof (ipv4_t))
    return false;

  if ((hdr = malloc (sizeof (ipv4_t))) == NULL)
    {
      free (hdr);
      return false;
    }

  /*
  hdr->ihl = 5;
  hdr->ver = 4;
  hdr->tos = htons (0x2);
  hdr->len = 0;
  hdr->indent = htons (rand ());
  hdr->offset = 0;
  hdr->ttl = 181;
  hdr->proto = proto;
  hdr->src_addr = src;
  hdr->dest_addr = dst;
  hdr->checksum = 0;
  */

  args->net_layer.ipv4 = args->packet + args->plen;
  memcpy (args->net_layer.ipv4, hdr, sizeof (ipv4_t));
  args->plen += sizeof (ipv4_t);

  free (hdr);

  return true;
}
