#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

#ifndef __section
# define __section(NAME)                  \
__attribute__((section(NAME), used))
#endif

/*
 * clang -O2 -target bpf -c xdp_test.c -o xdp_prog.o
 * sudo ip link set dev wlan0-virt xdp obj xdp_prog.o
 * sudo ip link set dev wlan0-virt xdp off
 */

__section("prog")
int
xdp_drop (struct xdp_md *ctx)
{
  return XDP_DROP;
}

char __license[] __section("license") = "GPL";

/*
enum xdp_action {
  XDP_ABORTED = 0,
  XDP_DROP,
  XDP_PASS,
  XDP_TX,
  XDP_REDIRECT,
};
*/