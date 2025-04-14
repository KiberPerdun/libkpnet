//
// Created by KiberPerdun on 3/12/25.
//

#include "if_packet.h"

u0
setup_bpf_filter (i32 fd, u32 ip, u16 type)
{
  struct sock_filter bpf_filter[] =
{
    BPF_STMT (BPF_LD + BPF_W + BPF_ABS, 12),
    BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K, ip, 0, 1),
    BPF_STMT (BPF_LD + BPF_B + BPF_ABS, 23),
    BPF_JUMP (BPF_JMP + BPF_JEQ + BPF_K, type, 0, 1),
    BPF_STMT (BPF_RET + BPF_K, 0),
    BPF_STMT (BPF_RET + BPF_K, 0xFFFFFFFF),
  };

  struct sock_fprog filter;
  filter.len = sizeof (bpf_filter) / sizeof (bpf_filter[0]);
  filter.filter = bpf_filter;

  if (setsockopt (fd, SOL_SOCKET, SO_ATTACH_FILTER, &filter, sizeof(filter)) < 0)
    {
      perror("setsockopt SO_ATTACH_FILTER");
      exit(EXIT_FAILURE);
    }
}
