//
// Created by KiberPerdun on 25.10.24.
//

#include "tcp.h"
#include "if_packet.h"

/*
bool
build_tcp_init_hdr (u0 *ars)
{
  connection_args_t *args;
  tcp_t *hdr;

  args = ars;

  if (args->packet == NULL)
    return false;

  if (MAX_PACKET_LEN < args->plen + sizeof (tcp_t))
    return false;

  if ((hdr = build_tcp_raw (2856147168, 0, 0x2, 64240, 0, 0, NULL, args)) == NULL)
    {
      free (hdr);
      return false;
    }

  if (args->net_layer.ipv4->ver == 4)
    {
      pseudo_t psh;

      if (args->tp_layer.tcp != NULL)
        args->plen = (u16) ((u0 *) args->tp_layer.tcp - args->packet);

      args->tp_layer.tcp = args->packet + args->plen;
      memcpy (args->tp_layer.tcp, hdr, sizeof (tcp_t));
      args->plen += sizeof (tcp_t);

      psh.source_address = args->net_layer.ipv4->src_addr;
      psh.dest_address = args->net_layer.ipv4->dest_addr;
      psh.placeholder = 0;
      psh.protocol = IPPROTO_TCP;
      psh.tcp_length = htons (sizeof (tcp_t));

      u8 *buffer
          = malloc (sizeof (pseudo_t) + sizeof (tcp_t));
      memcpy (buffer, &psh, sizeof (pseudo_t));
      memcpy (buffer + sizeof (pseudo_t), hdr,
              sizeof (tcp_t));

      args->tp_layer.tcp->check = tcp_checksum ((u16 *) buffer, sizeof (pseudo_t) + sizeof (tcp_t));
      args->net_layer.ipv4->len = htons (sizeof (ipv4_t) + sizeof (tcp_t));

      args->net_layer.ipv4->checksum = 0;
      args->net_layer.ipv4->checksum = in_check ((u16 *) (args->net_layer.ipv4), sizeof (ipv4_t));

      free (buffer);
    }

  else
    return false;

  free (hdr);

  return true;
}
*/
