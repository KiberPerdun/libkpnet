#include "eth.h"
#include "pthread.h"
#include "types.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "sys/mman.h"
#include "dhcp.h"
#include <arpa/inet.h>
#include "linux/genetlink.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

i32
main (u0)
{
  pthread_t client, server;

#include "netlink.h"

  create_veth_pair ("libkpnet_c", "libkpnet_s");
  /* delete_if (get_ifid ("libkpnet_c")); */
  /* delete_if (get_ifid ("libkpnet_s")); */
  up_if (get_ifid ("libkpnet_c"));
  up_if (get_ifid ("libkpnet_s"));

  const u32 src_ip = inet_addr ("192.168.1.2");
  const u32 dst_ip = inet_addr ("192.168.1.3");
  bind_if (get_ifid ("libkpnet_c"), src_ip);
  bind_if (get_ifid ("libkpnet_s"), dst_ip);

  if (pthread_create (&client, NULL, create_client, NULL) != 0)
    return 0;

  if (pthread_create (&server, NULL, create_server, NULL) != 0)
    return 0;

  if (pthread_join (client, NULL) != 0)
    return 0;

  if (pthread_join (server, NULL) != 0)
    return 0;
}
