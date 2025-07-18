//
// Created by KiberPerdun on 7/18/25.
//

#include "netlink.h"

i32 get_ifid (const char *name)
{
  struct rtattr *rta;
  struct ifinfomsg *ifinfo;
  struct nlmsghdr *nlmsg;
  i32 fd;
  u0 *req, *resp;

  req = calloc (1, 4096);
  resp = calloc (1, 4096);
  fd = netlink_open (name);

  nlmsg = req;
  nlmsg->nlmsg_type = RTM_GETLINK;
  nlmsg->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
  nlmsg->nlmsg_seq = 1;
  nlmsg->nlmsg_pid = getpid ();
  nlmsg->nlmsg_len = NLMSG_LENGTH (sizeof (struct ifinfomsg));

  ifinfo = NLMSG_DATA (nlmsg);
  ifinfo->ifi_family = AF_UNSPEC;
  ifinfo->ifi_index = 0;
  ifinfo->ifi_change = 0;
  ifinfo->ifi_flags = 0;
  ifinfo->ifi_type = 1;

  rta = IFLA_RTA (ifinfo);
  rta->rta_len = RTA_LENGTH (strlen (name) + 1);
  rta->rta_type = IFLA_IFNAME;
  nlmsg->nlmsg_len += rta->rta_len;
  memcpy (RTA_DATA (rta), name, strlen (name) + 1);

  send (fd, req, nlmsg->nlmsg_len, 0);
  recv (fd, resp, 4096, 0);
  nlmsg = resp;
  ifinfo = NLMSG_DATA (nlmsg);

  netlink_close (fd);
  fd = ifinfo->ifi_index;
  free (req);
  free (resp);
  return fd;
}
