#include "eth.h"
#include "if_packet.h"
#include "ipv4.h"
#include "tcp.h"
#include "types.h"
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

i32
main (u0)
{
  connection_args_t *args;
  u0 *packet = NULL;
  eth_t *eth;
  u16 src_port, dst_port;

  eth = eth_open ("wlan0-peer");

  time_t t;
  srand ((u32) time (&t));

  if (!(args = calloc (1, sizeof (connection_args_t))))
    goto cleanup;

  if (!(packet = calloc (1, MAX_PACKET_LEN)))
    goto cleanup;

  dst_port = 443 + rand () % 65000;
  src_port = 12345; /* 443 + rand () % 65000 */

  args->srcport = htons (src_port);
  args->dstport = htons (dst_port);
  args->proto = IPPROTO_TCP;
  args->packet = packet;
  args->eth = eth;
  args->plen = 0;
  args->TCP_STATUS = TCP_LISTEN;
  args->tp_layer.tcp = NULL;

  build_mac_client_raw ("46:9e:59:1e:5a:86", "5a:50:12:f7:b0:f5",
                               ETHERTYPE_IP, args);

  /* FIN ACK 0x011 */

  fill_ipv4 (inet_addr ("192.168.1.2"), inet_addr ("192.168.1.3"), IPPROTO_TCP, args);

  build_tcp_init_hdr (args);

  recv_filtered (args->eth->fd, if_ipv4_tcp, args);

  build_tcp_syn_ack_hdr (args);

  eth_send (args->eth, args->packet, args->plen);

  args->tp_layer.tcp->seq += htonl (1);

  args->plen = sizeof (mac_t) + sizeof (ipv4_t);

  for (;;)
    {
      recv_filtered (args->eth->fd, if_ipv4_tcp, args);

      args->plen = sizeof (mac_t) + sizeof (ipv4_t);

      build_tcp_ack_hdr (args);

      eth_send (args->eth, args->packet, args->plen);
    }

  /*

  tcp_make_handshake (args);

  args->plen = sizeof (mac_t) + sizeof (ipv4_t);

  args->payload = malloc (MAX_PACKET_LEN - sizeof (mac_t) - sizeof (ipv4_t)
                          - sizeof (tcp_t));

  for (;;)
    {
      fgets (args->payload,
             MAX_PACKET_LEN - sizeof (mac_t) - sizeof (ipv4_t)
                 - sizeof (tcp_t),
             stdin);


      build_tcp_payload_hdr (args);

      eth_send (args->eth, args->packet, args->plen);

      args->tp_layer.tcp->seq += ntohl (strlen (args->payload) - 1);
      args->plen = sizeof (mac_t) + sizeof (ipv4_t);
    }

  */

  free (args->payload);

  eth_close (eth);

cleanup:
  free (args);
  free (packet);
}
