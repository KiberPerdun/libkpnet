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
  u0 *packet;
  eth_t *e;
  u16 src_port, dst_port;
  mac_t *mac;

  e = eth_open ("wlan0");

  time_t t;
  srand ((u32) time (&t));

  if (!(args = calloc (1, sizeof (connection_args_t))))
    goto cleanup;

  if (!(packet = calloc (1, 1500)))
    goto cleanup;

  dst_port = 80;
  src_port = 443 + rand () % 65000;

  args->srcport = htons (src_port);
  args->dstport = htons (dst_port);
  args->proto = IPPROTO_TCP;
  args->packet = packet;
  args->eth = e;
  args->plen = 0;
  args->tp_layer.tcp.TCP_STATUS = TCP_CLOSED;

  build_mac_client_raw ("9a:60:ca:f3:df:64", "50:28:4a:b4:bf:2c",
                               ETHERTYPE_IP, args);

  /* FIN ACK 0x011 */

  fill_ipv4 (inet_addr ("172.20.10.8"), inet_addr ("64.233.165.113"), 0x06, args);

  build_tcp_init_hdr (args);

  eth_send (e, args->packet, args->plen);

  recv_filtered (e->fd, if_ipv4_tcp, args);

  build_tcp_ack_hdr (args);

  eth_send (e, args->packet, args->plen);

  /* tcp_make_handshake (args, e); */

  /* tcp_get_html (args, e); */

  eth_close (e);

cleanup:
  free (args);
  free (packet);
  free (mac);
}
