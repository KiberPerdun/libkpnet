//
// Created by KiberPerdun on 10.01.26.
//

#include "xdp.h"

u0
umem_stack_push (umem_stack_t *stack, u64 addr)
{
  if (__builtin_expect (stack->top == stack->cap, 0))
    return;

  stack->addrs[stack->top++] = addr;
}
