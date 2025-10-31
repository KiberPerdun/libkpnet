#include "dhcp.h"
#include "eth.h"
#include "md5.h"
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

  create_veth_pair (CLIENT_INAME, SERVER_INAME);
  /* delete_if (get_ifid ("libkpnet_c")); */
  /* delete_if (get_ifid ("libkpnet_s")); */
  up_if (get_ifid (CLIENT_INAME));
  up_if (get_ifid (SERVER_INAME));

  /*
  i32 a, b, k;

  a = 69;
  b = 6;
  k = 29;
  k -= 7;

  asm volatile ("loop:;"
                "subl $7, %[k];"

                "movl %[a], %%eax;"
                "movl %[b], %%ebx;"
                "subl %%ebx, %%eax;"
                "cdq;"
                "xor %%edx, %%eax;"
                "sub %%edx, %%eax;"
                "movl %%eax, %[a];"

                "cmpl $9, %[k];"
                "jl nazi;"

                "jmp loop;"

                "nazi:;"
                "leal (%[a], %[k]), %[b];"
                : [a] "+r"(a), [b] "+r"(b), [k] "+r"(k)
                :
                : "eax", "ebx", "edx");

  printf ("a = %d, b = %d, k = %d\n", a, b, k);
  return 0;
   */

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
