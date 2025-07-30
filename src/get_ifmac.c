//
// Created by KiberPerdun on 7/31/25.
//

#include "netlink.h"

i32
get_ifmac (const char *name, u8 *mac)
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
  rta = IFLA_RTA (ifinfo);
  for (;rta->rta_type != 1;)
    rta = RTA_NEXT (rta, nlmsg->nlmsg_len);

  memcpy (mac, RTA_DATA (rta), 6);

  netlink_close (fd);
  free (req);
  free (resp);
  return 1;
}
