//
// Created by KiberPerdun on 14.11.24.
//

#ifndef LIBKPNET_IF_PACKET_H
#define LIBKPNET_IF_PACKET_H

#include "types.h"

#define MAX_PACKET_LEN 1516

typedef struct node {
  u16 data;
  struct node *next;
} node_t;

/* L2 -> ? -> L3 -> ? -> ... -> [payload]<fragmented data><signaling> -> aee */
typedef enum PROTO_STACK_TYPE
{
  PROTO_STACK_MAC80211_IP_TCP,
  PROTO_STACK_MAC80211_IP_SCTP,
  PROTO_STACK_MAC8023_IP_TCP,
  PROTO_STACK_MAC8023_IP_SCTP,
  PROTO_STACK_IP_TCP,
  PROTO_STACK_IP_SCTP,
  PROTO_STACK_IP_UDP,
  PROTO_STACK_TCP_RAW,
  PROTO_STACK_TCP,
  PROTO_STACK_IP_RAW,
  PROTO_STACK_IP,
  PROTO_STACK_ARP,
} PROTO_STACK_TYPE_T;

typedef struct frame_sync_ip_tcp
{
  u32 ip_check_part;
  u32 tcp_check_part;
} frame_sync_ip_tcp_t;

#include <stdbool.h>

#include <setjmp.h>

typedef struct frame_data
{
  u0 *packet;
  u16 plen;
  PROTO_STACK_TYPE_T proto;
  u0 *sync;
  u0 *state;
} frame_data_t;

#include <stdbool.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ioctl.h>
#include "mac.h"
#include "tcp.h"
#include "ipv4.h"
#include "ipv6.h"
#include "stdbool.h"
#include "udp.h"
#include "eth.h"
#include <linux/io_uring.h>
#include <linux/filter.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "linux/netlink.h"
#include "linux/rtnetlink.h"
#include <stdatomic.h>
#include "sctp.h"

typedef struct ipv4_hdr {
#if __BYTE_ORDER__ == __LITTLE_ENDIAN
  u8  ihl:4;
  u8  ver:4;
#endif
#if __BYTE_ORDER__ == __BIG_ENDIAN__
  u8  ver:4;
  u8  ihl:4;
#endif

  u8  tos;
  u16 len;

  u16 indent;
  u16 offset;

  u8  ttl;
  u8  proto;
  u16 check;
  u32 src_addr;
  u32 dest_addr;
} ipv4_t;

typedef struct connection_ip_sctp_state
{
  SCTP_STATUS_T status;
} connection_ip_sctp_state_t;

typedef struct connection_args
{
  /* For general purpose */
  char *payload;
  u32 srcport;
  u32 dstport;
  u0 *packet;
  eth_t *eth;
  u16 plen;
  u8 proto;

  /* SCTP only */
  struct
  {
    bool slave;
    u32 src_ver_tag;
    u32 dst_ver_tag;
    u32 src_tsn;
    u32 dst_tsn;

    u32 self_a_rwnd;
    u32 dest_a_rwnd;

    u16 self_os;
    u16 self_mis;

    u16 dest_os;
    u16 dest_mis;

    u64 hmac;

    /* Error */
    node_t *errors;
  } sctp_connection;

  union
  {
    ipv4_t *ipv4;
    ipv6_t *ipv6;
  } net_layer;

  enum
  {
    TCP_LISTEN = 1,
    TCP_SYN_SENT = 2,
    TCP_SYN_RECEIVED = 3,
    TCP_ESTABLISHED_CONNECTON = 4,
    TCP_FIN_WAIT_1 = 5,
    TCP_FIN_WAIT_2 = 6,
    TCP_CLOSE_WAIT = 7,
    TCP_CLOSING = 8,
    TCP_LAST_ACK = 9,
    TCP_TIME_WAIT = 10,
    TCP_CLOSED = 11,
  } TCP_STATUS;

  /* May better soon, maybe. */
  SCTP_STATUS_T SCTP_STATUS;

  union
  {
    /* tcp_t *tcp; */
    udp_t *udp;
    sctp_t *sctp;
  } tp_layer;
} connection_args_t;

typedef struct if_ip_tcp_meta
{
  i32 state;
  u32 src_ip;
  u32 dst_ip;
  u16 src_port;
  u16 dst_port;

  u32 dst_seq;
  u32 src_seq;
} if_ip_tcp_meta_t;

typedef struct if_ip_sctp_meta
{
  int state;
  u32 src_ip;
  u32 dst_ip;
  u16 src_port;
  u16 dst_port;

  u32 dst_tsn;
  u32 src_ver_tag;
  u32 dst_arwnd;
  u32 src_arwnd;
  u16 src_os;
  u16 src_mis;
  u16 dst_os;
  u16 dst_mis;

  u32 src_tsn;
  u32 dst_ver_tag;

  char *add;
} if_ip_sctp_meta_t;

typedef struct connection_sctp_state
{
  atomic_bool shutdown_requested;
  atomic_bool packet_proccessing;
} connection_sctp_state_t;

typedef bool (*lrcall_t)(u0 *, u64, connection_args_t *);
typedef u0*   (*ifcall)(u0 *, u16, u0 *);

u0   recv_packet (i32 fd, ifcall filter, u0 *meta);
u0   recv_filtered (i32 fd, lrcall_t filter, connection_args_t * args);
bool if_ipv4 (u0 *packet, u64 size, connection_args_t *args);
bool if_tcp (u0 *packet, u64 size, connection_args_t *args);
bool if_sctp (u0 *packet, u64 size, connection_args_t *args);
bool if_ipv4_tcp (u0 *packet, u64 size, connection_args_t *args);
bool if_ipv4_sctp (u0 *packet, u64 size, connection_args_t *args);
u0*  if_ip_tcp (u0 *packet, u16 size, u0 *meta);
u0*  if_ip_sctp (u0 *packet, u16 size, u0 *meta);
#endif // LIBKPNET_IF_PACKET_H
