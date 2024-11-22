#include "ipv4.h"
#include "malloc.h"
#include <netinet/in.h>
#include <stdlib.h>
#include "if_packet.h"

int
ip_check_add (const void *buf, size_t len, int check)
{
  u16 *sp = (u16 *)buf;
  int n, sn;

  sn = len / 2;
  n = (sn + 15) / 16;

  switch (sn % 16)
    {
    case 0:
      do
        {
          check += *sp++;
        case 15:
          check += *sp++;
        case 14:
          check += *sp++;
        case 13:
          check += *sp++;
        case 12:
          check += *sp++;
        case 11:
          check += *sp++;
        case 10:
          check += *sp++;
        case 9:
          check += *sp++;
        case 8:
          check += *sp++;
        case 7:
          check += *sp++;
        case 6:
          check += *sp++;
        case 5:
          check += *sp++;
        case 4:
          check += *sp++;
        case 3:
          check += *sp++;
        case 2:
          check += *sp++;
        case 1:
          check += *sp++;
        }
      while (--n > 0);
    }

  if (len & 1)
    check += htons (*(u_char *)sp << 8);

  return check;
}

u16
in_check (u16 *ptr, int nbytes)
{
  int sum;
  sum = ip_check_add (ptr, nbytes, 0);
  return ip_check_carry (sum);
}

bool
fill_ipv4 (u32 src, u32 dst, u8 proto, u0 *ars)
{
  connection_args_t *args;
  ipv4_t *hdr;

  args = ars;

  if (args->packet == NULL)
    return false;

  if (!(MAX_PACKET_LEN >= args->plen + sizeof (ipv4_t)))
    return false;

  if ((hdr = malloc (sizeof (ipv4_t))) == NULL)
    {
      free (hdr);
      return false;
    }

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

  args->net_layer.ipv4 = args->packet + args->plen;
  memcpy (args->net_layer.ipv4, hdr, sizeof (ipv4_t));
  args->plen += sizeof (ipv4_t);

  free (hdr);

  return true;
}
