//
// Created by KiberPerdun on 14.11.24.
//

#include "if_packet.h"

bool
if_tcp (u0 *packet, u64 size, connection_args_t *args)
{
  ipv4_t *ip;
  tcp_t *buf;
  u8 offset;
  u0 *opts;
  u16 pk_check;

  buf = packet;

  if (size >= sizeof (tcp_t))
    {
      offset = (ntohs (buf->flags) >> 12) & 0XF;
      ip = packet - sizeof (ipv4_t);

      if (size < offset * 4)
        return false;

      pk_check = buf->check;
      buf->check = 0;

      pseudo_t psh;
      psh.source_address = ip->src_addr;
      psh.dest_address = ip->dest_addr;
      psh.placeholder = 0;
      psh.protocol = ip->proto;

      psh.tcp_length = htons (offset * 4);

      u8 *buffer = malloc (sizeof (pseudo_t) + offset * 4);
      memcpy (buffer, &psh, sizeof (pseudo_t));
      memcpy (buffer + sizeof (pseudo_t), buf, offset * 4);

      if (pk_check != tcp_checksum ((u16 *) buffer, sizeof (pseudo_t) + offset * 4))
        {
          free (buffer);
          return false;
        }

      free (buffer);

      printf ("{tcp}(seq: %u, ack: %u, win: %hu, urgent: %hu, flags: %hu, offset: %hu);\n",
              ntohl (buf->seq),
              ntohl (buf->ack),
              ntohs (buf->win),
              ntohs (buf->urgent),
              ntohs (buf->flags) & 0x1FF,
              offset);
      


      if ((ntohs (buf->flags) & 0x1FF) == 0x12 && args->tp_layer.tcp.TCP_STATUS == TCP_CLOSED)
        {
          args->tp_layer.tcp.TCP_STATUS = TCP_SYN_RECEIVED;
          args->tp_layer.tcp._tcp->seq = ntohl (buf->ack);
          args->tp_layer.tcp._tcp->ack = ntohl (buf->seq);
        }
      /*
      else if (((ntohs (buf->flags) & 0x1ff) == 0x18 || (ntohs (buf->flags) & 0x1ff) == 0x10) && args->tp_layer.tcp.TCP_STATUS == TCP_ESTABLISHED)
        {
          if (size <= (sizeof (ipv4_t) + sizeof (tcp_t)))
            return false;

          args->data_len = size - (sizeof (ipv4_t) + sizeof (tcp_t) + (offset - 4) * 4);
          printf ("%u %u", args->data_len, size);
          fflush (stdout);
          args->data = malloc (args->data_len + 1);
          ((u8 *) args->data)[args->data_len] = '\0';
          memcpy (args->data, packet + 14 + sizeof (ipv4_t) + sizeof (tcp_t) + ((offset - 4) * 4), args->data_len);
          return true;
        }
        */

      offset -= 5;
      opts = buf;
      opts += sizeof (tcp_t);


      for (u8 i = 0; i < offset; ++i)
        {
          printf("{tcp_opt_");

          switch (((u8 *)opts)[0])
            {
            case 2:
              printf ("mss}");
              break;
            default:
              printf ("unrecognized}");
              break;
            }

          printf ("(kind: %hu, len: %hu, val: %hu)\n", ((u8 *)opts)[0], ((u8 *)opts)[1], htons (((u16 *)opts)[1]));
          opts += (i + 1) * 4;
        }

      puts ("Packet end]\n");
      fflush (stdout);
      return true;
    }

  return false;
}
