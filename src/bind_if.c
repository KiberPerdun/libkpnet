//
// Created by KiberPerdun on 7/18/25.
//

#include "netlink.h"

#include <arpa/inet.h>

i32 bind_if (i32 intf_index, u32 ip)
{
  struct rtattr *rta;
  struct in_addr addr;
  struct ifaddrmsg *ifa;
  struct ifinfomsg *ifinfo;
  struct nlmsghdr *nlmsg;
  u0 *req;
  i32 fd;

  req = calloc (1, 4096);
  fd = netlink_open ();
  nlmsg = req;
  nlmsg->nlmsg_type = RTM_NEWADDR;
  nlmsg->nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_ACK;
  nlmsg->nlmsg_seq = 1;
  nlmsg->nlmsg_pid = getpid ();
  nlmsg->nlmsg_len = NLMSG_LENGTH (sizeof (struct ifaddrmsg));

  ifa = NLMSG_DATA (nlmsg);
  ifa->ifa_family = AF_INET;
  ifa->ifa_prefixlen = 24;
  ifa->ifa_flags = 0;
  ifa->ifa_scope = RT_SCOPE_UNIVERSE;
  ifa->ifa_index = intf_index;

  rta = IFA_RTA (ifa);
  rta->rta_type = IFA_LOCAL;
  rta->rta_len = RTA_LENGTH (sizeof (struct in_addr));
  memcpy (RTA_DATA(rta), &ip, sizeof (struct in_addr));
  nlmsg->nlmsg_len += RTA_ALIGN(rta->rta_len);

  rta = (u0 *) nlmsg + NLMSG_ALIGN(nlmsg->nlmsg_len);
  rta->rta_type = IFA_ADDRESS;
  rta->rta_len = RTA_LENGTH (sizeof (struct in_addr));
  memcpy (RTA_DATA (rta), &ip, sizeof (struct in_addr));
  nlmsg->nlmsg_len += RTA_ALIGN(rta->rta_len);

  send (fd, req, nlmsg->nlmsg_len , 0);
  netlink_close (fd);
  free (req);
}
