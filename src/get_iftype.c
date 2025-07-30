//
// Created by KiberPerdun on 7/31/25.
//

#include "netlink.h"

i32
get_iftype (const char *name, u16 type, u0 *buf, u16 buflen)
{
  struct rtattr *rta;
  struct ifinfomsg *ifinfo;
  struct nlmsghdr *nlmsg;
  i32 fd, rlen;
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
  rlen = IFLA_PAYLOAD (nlmsg);
  for (; RTA_OK (rta, rlen); rta = RTA_NEXT (rta, rlen))
    if (rta->rta_type == type)
      {
        if (rta->rta_len - RTA_LENGTH (0) > buflen)
          {
            netlink_close (fd);
            free (req);
            free (resp);
            return -1;
          }

        memcpy (buf, RTA_DATA (rta), rta->rta_len - RTA_LENGTH (0));
        netlink_close (fd);
        fd = rta->rta_len - RTA_LENGTH (0);
        free (req);
        free (resp);
        return fd;
      }

  netlink_close (fd);
  free (req);
  free (resp);
  return 1;
}
