// xdp_test.bpf.c
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>

struct {
  __uint(type, BPF_MAP_TYPE_XSKMAP);
  __type(key, __u32);
  __type(value, __u32);
  __uint(max_entries, 64);
} xsks_map SEC(".maps");

SEC("xdp")
int kp_xdp_redirect(struct xdp_md *ctx)
{
  __u32 index = ctx->rx_queue_index;

  if (bpf_map_lookup_elem(&xsks_map, &index))
    return bpf_redirect_map(&xsks_map, index, 0);

  return XDP_PASS;
}

char LICENSE[] SEC("license") = "GPL";
