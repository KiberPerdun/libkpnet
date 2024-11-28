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
  eth_t *e;
  u16 src_port, dst_port;

  e = eth_open ("wlan0");

  time_t t;
  srand ((u32) time (&t));

  if (!(args = calloc (1, sizeof (connection_args_t))))
    goto cleanup;

  if (!(packet = malloc (1500)))
    goto cleanup;

  memset (packet, 0, 1500);

  dst_port = 80;
  src_port = 443 + rand () % 65000;

  args->srcport = htons (src_port);
  args->dstport = htons (dst_port);
  args->proto = IPPROTO_TCP;
  args->packet = packet;
  args->eth = e;
  args->plen = 0;
  args->TCP_STATUS = TCP_CLOSED;
  args->tp_layer.tcp = NULL;

  build_mac_client_raw ("9c:a2:f4:a2:a9:3f", "50:28:4a:b4:bf:2c",
                               ETHERTYPE_IP, args);

  /* FIN ACK 0x011 */

  fill_ipv4 (inet_addr ("192.168.0.108"), inet_addr ("64.233.165.113"), 0x06, args);

  tcp_make_handshake (args);

  tcp_get_html (args);

  eth_close (e);

cleanup:
  free (args);
  free (packet);
}
