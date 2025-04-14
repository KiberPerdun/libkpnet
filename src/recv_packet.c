//
// Created by KiberPerdun on 3/12/25.
//

#include "if_packet.h"
#include "linux/io_uring.h"
#include "sys/syscall.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/io_uring.h>
#include <sys/syscall.h>
#include <sys/mman.h>

#define QUEUE_DEPTH 128

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/io_uring.h>
#include <sys/syscall.h>
#include <sys/mman.h>

#define QUEUE_DEPTH 128
#define BUFFER_SIZE 1024

u0
recv_packet (i32 fd, ifcall filter)
{
  u0 *buf = malloc (MAX_PACKET_LEN);


  free (buf);
}
