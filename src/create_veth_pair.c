//
// Created by KiberPerdun on 7/17/25.
//

#include "netlink.h"

i32 create_veth_pair (const char *src, const char *dst)
{
  struct rtattr *rta, *linkinfo, *pinfi, *nfdata, *pinaatr;
  struct ifinfomsg *ifinfo, *ifpifi;
  struct nlmsghdr *nlmsg;
  const char ikf[] = "veth";
  u0 *req;
  i32 fd;

  fd = netlink_open ();
  req = calloc (1, 4096);

  nlmsg = req;

  nlmsg->nlmsg_type = RTM_NEWLINK;
  nlmsg->nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_ACK | NLM_F_EXCL;
  nlmsg->nlmsg_seq = 1;
  nlmsg->nlmsg_pid = getpid ();
  nlmsg->nlmsg_len = NLMSG_LENGTH (sizeof (struct ifinfomsg));

  ifinfo = NLMSG_DATA (nlmsg);
  ifinfo->ifi_family = AF_INET;
  ifinfo->ifi_index = 0;
  ifinfo->ifi_change = 0;
  ifinfo->ifi_type = 0;

  rta = IFLA_RTA (ifinfo);
  rta->rta_len = RTA_LENGTH (strlen (src) + 1);
  rta->rta_type = IFLA_IFNAME;
  nlmsg->nlmsg_len += rta->rta_len;
  memcpy (RTA_DATA (rta), src, strlen (src) + 1);

  linkinfo = (u0 *) nlmsg + NLMSG_ALIGN (nlmsg->nlmsg_len);
  linkinfo->rta_type = IFLA_LINKINFO;
  linkinfo->rta_len = RTA_LENGTH (0);
  nlmsg->nlmsg_len = NLMSG_ALIGN (nlmsg->nlmsg_len) + linkinfo->rta_len;

  rta = (u0 *) linkinfo + linkinfo->rta_len;
  rta->rta_type = IFLA_INFO_KIND;
  rta->rta_len = RTA_LENGTH (strlen (ikf) + 1);
  linkinfo->rta_len += RTA_ALIGN (rta->rta_len);
  nlmsg->nlmsg_len += RTA_ALIGN (rta->rta_len);
  memcpy (RTA_DATA (rta), ikf, strlen (ikf) + 1);

  nfdata = (u0 *) rta + RTA_ALIGN (rta->rta_len);
  nfdata->rta_type = IFLA_INFO_DATA;
  nfdata->rta_len = RTA_LENGTH (0);

  pinfi = (u0 *) nfdata + RTA_LENGTH(0);
  pinfi->rta_type = 1;
  pinfi->rta_len = RTA_LENGTH (sizeof (struct ifinfomsg));

  ifpifi = RTA_DATA (pinfi);
  ifpifi->ifi_family = AF_UNSPEC;
  ifpifi->ifi_index = 0;
  ifpifi->ifi_type = 0;
  ifpifi->ifi_change = 0;
  ifpifi->ifi_flags = 0;

  pinaatr = (u0 *) ifpifi + NLMSG_ALIGN (sizeof (struct ifinfomsg));
  pinaatr->rta_type = IFLA_IFNAME;
  pinaatr->rta_len = RTA_LENGTH (strlen (dst) + 1);
  memcpy (RTA_DATA (pinaatr), dst, strlen (dst) + 1);

  pinfi->rta_len += RTA_ALIGN (pinaatr->rta_len);
  nfdata->rta_len += RTA_ALIGN (pinfi->rta_len);
  linkinfo->rta_len += RTA_ALIGN (nfdata->rta_len);
  nlmsg->nlmsg_len = NLMSG_ALIGN (nlmsg->nlmsg_len) + RTA_ALIGN (linkinfo->rta_len) - RTA_LENGTH (0);

  send (fd, req, nlmsg->nlmsg_len, 0);

  netlink_close (fd);
  free (req);
}
