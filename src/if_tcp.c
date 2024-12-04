//
// Created by KiberPerdun on 14.11.24.
//

#include "if_packet.h"
#include <stdlib.h>
#include <time.h>

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

      if (((ntohs (buf->flags) & 0x1FF) == 0x18) && args->TCP_STATUS == TCP_ESTABLISHED_CONNECTON)
      {
        args->tp_layer.tcp->seq = buf->ack;
        args->tp_layer.tcp->ack = buf->seq;

        printf ("msg: ");
        for (u32 i = 0; i < ntohs (ip->len) - sizeof (tcp_t) - sizeof (ipv4_t); ++ i)
          {
            putc (((u8 *)buf + sizeof (tcp_t))[i], stdout);
            args->tp_layer.tcp->ack += htonl (1);
          }

        putc ('\n', stdout);

        return true;
      }

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

      if (((ntohs (buf->flags) & 0x1FF) == 0x12) && args->TCP_STATUS == TCP_SYN_SENT)
        {
          args->TCP_STATUS = TCP_SYN_RECEIVED;
          args->tp_layer.tcp->seq = buf->ack;
          args->tp_layer.tcp->ack = buf->seq;
        }
      else if (((ntohs (buf->flags) & 0x1FF) == 0x2) && args->TCP_STATUS == TCP_LISTEN)
        {
          args->TCP_STATUS = TCP_TIME_WAIT;
          time_t t;
          srand ((u32) time (&t));
          args->tp_layer.tcp->seq = rand ();
          args->tp_layer.tcp->ack = buf->seq;
        }
      else if (((ntohs (buf->flags) & 0x1FF) == 0x10) && args->TCP_STATUS == TCP_TIME_WAIT)
        {
          args->TCP_STATUS = TCP_ESTABLISHED_CONNECTON;
          args->net_layer.ipv4->dest_addr = ip->src_addr;
          args->tp_layer.tcp->seq = buf->ack;
          args->tp_layer.tcp->ack = buf->seq;
        }
      else
        return false;

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
