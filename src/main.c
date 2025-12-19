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

u0
a (u0)
{
  puts ("goida");
}

i32
main (u0)
{
  pthread_t client, server;

#include "netlink.h"

  /*
  ringbuf_t *events_allocator = create_allocator (64, 256);
  ringtimer_t *timer = create_ringtimer (8, events_allocator);
  insert_ringtimer (&a, 69, timer);
  insert_ringtimer (&a, 69, timer);
  insert_ringtimer (&a, 69, timer);
  insert_ringtimer (&a, 69, timer);
  insert_ringtimer (&a, 69, timer);
  insert_ringtimer (&a, 69, timer);
  tick_ringtimer (timer);

  for (i32 i = 0; i < 100; ++i)
    {
      printf ("tick: %d\n", i + 1);
      tick_ringtimer (timer);
    }

  free_ringbuf (events_allocator);
  free (timer->timers);
  free (timer);

   */
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

  /*
  if (pthread_create (&server, NULL, create_server, NULL) != 0)
    return 0;
    */

  if (pthread_join (client, NULL) != 0)
    return 0;

  /*
  if (pthread_join (server, NULL) != 0)
    return 0;
    */
}
