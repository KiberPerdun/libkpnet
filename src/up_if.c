//
// Created by KiberPerdun on 7/18/25.
//

#include "netlink.h"

i32 up_if (i32 intf_index)
{
  struct ifinfomsg *ifinfo;
  struct nlmsghdr *nlmsg;
  u0 *req;
  i32 fd;

  req = calloc (1, 4096);
  fd = netlink_open ();
  nlmsg = req;
  nlmsg->nlmsg_type = RTM_NEWLINK;
  nlmsg->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
  nlmsg->nlmsg_seq = 1;
  nlmsg->nlmsg_pid = getpid ();

  ifinfo = NLMSG_DATA (nlmsg);
  ifinfo->ifi_family = AF_UNSPEC;
  ifinfo->ifi_index = intf_index;
  ifinfo->ifi_change = IFF_UP;
  ifinfo->ifi_flags = IFF_UP;
  ifinfo->ifi_type = 1;

  nlmsg->nlmsg_len = NLMSG_ALIGN (sizeof (struct nlmsghdr) + sizeof (struct ifinfomsg));

  send (fd, req, nlmsg->nlmsg_len , 0);
  netlink_close (fd);
  free (req);
}

