//
// Created by KiberPerdun on 9/16/24.
//

#ifndef LIBKPNET_ARP_H
#define LIBKPNET_ARP_H

#include "mac.h"
#include "types.h"

#define ARP_HDR_LEN 8    /* base ARP header length */
#define ARP_ETHIP_LEN 20 /* base ARP message length */

#define BROADCAST_MAC 0x0000FFFFFFFFFFFF

#define ARP_HRD_ETHER 1

#define ARP_OP_REQUEST 1
#define ARP_OP_REPLY 2

#define ARP_PRO_IP 0x0800

#define IP_ADDR_LEN 4 /* IP address length */
#define ETH_ADDR_LEN 6

#define ARP_OP_REQUEST 1
#define ARP_OP_REPLY 2
#define ARP_OP_REVREQUEST 3
#define ARP_OP_REVREPLY 4

typedef struct arp_hdr
{
  u16 proto;
  u16 hrd;
  u8 hln;
  u8 pln;
  u16 op;
} arp_t;

typedef struct arp_ethip
{
  mac_t sha;
  u32 spa;
  mac_t tha;
  u32 tpa;
} arp_eth_t;

arp_t *arp_fill_hdr (void);
arp_eth_t *arp_fill_eth (void);

#endif // LIBKPNET_ARP_H
