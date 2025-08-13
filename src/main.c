#include "dhcp.h"
#include "eth.h"
#include "pthread.h"
#include "ring_buffer.h"
#include "sys/mman.h"
#include "types.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>

#include "linux/genetlink.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

i32
main (u0)
{
  pthread_t client, server;

#include "netlink.h"
#include "md5.h"
  /*
  const char* text1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567899";
  MD5_CTX context;
  u8 digest[16];

  MD5Init (&context);
  MD5Update (&context, (const u8 *)text1, strlen(text1));
  MD5Final (digest, &context);

  printf ("Строка: %s\n", text1);
  printf ("MD5 хеш: ");
  for (i32 i = 0; i < 16; ++i)
    printf("%02x", digest[i]);
  printf("\n\n");

  return 0;
  */

  ring_buffer_t *rb = init_ring_buffer ();
  free_ring_buffer (rb);

  create_veth_pair (CLIENT_INAME, SERVER_INAME);
  /* delete_if (get_ifid ("libkpnet_c")); */
  /* delete_if (get_ifid ("libkpnet_s")); */
  up_if (get_ifid (CLIENT_INAME));
  up_if (get_ifid (SERVER_INAME));

  const u32 src_ip = inet_addr ("192.168.1.2");
  const u32 dst_ip = inet_addr ("192.168.1.3");
  bind_if (get_ifid (CLIENT_INAME), src_ip);
  bind_if (get_ifid (SERVER_INAME), dst_ip);

  if (pthread_create (&client, NULL, create_client, NULL) != 0)
    return 0;

  if (pthread_create (&server, NULL, create_server, NULL) != 0)
    return 0;

  if (pthread_join (client, NULL) != 0)
    return 0;

  if (pthread_join (server, NULL) != 0)
    return 0;
}
