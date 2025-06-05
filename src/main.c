#include "eth.h"
#include "pthread.h"
#include "types.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>

/*
 sudo ip link add name wlan0-virt type veth peer name wlan0-peer;sudo ip link set wlan0-virt up;sudo ip link set wlan0-peer up;sudo ip addr add 192.168.1.2/24 dev wlan0-virt;sudo ip addr add 192.168.1.3/24 dev wlan0-peer

 sudo ip link delete wlan0-virt
 */

#include "sys/mman.h"
#include "linux/if_tun.h"
#include "linux/if.h"
#include "threads.h"
#include "linux/xdp_diag.h"
#include "linux/if_xdp.h"

#include "dhcp.h"

i32
main (u0)
{
  pthread_t client, server;
  u16 proto = IPPROTO_TCP;

  printf ("%lu", sizeof (dhcp_t));

  if (pthread_create (&client, NULL, (void *(*)(void *)) create_client, &proto) != 0)
    return 0;

  if (pthread_create (&server, NULL, (void *(*)(void *)) create_server, &proto) != 0)
    return 0;

  if (pthread_join (client, NULL) != 0)
    return 0;

  if (pthread_join (server, NULL) != 0)
    return 0;

}
