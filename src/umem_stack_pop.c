//
// Created by KiberPerdun on 10.01.26.
//

#include "xdp.h"

u64
umem_stack_pop (umem_stack_t *stack)
{
  if (__builtin_expect (stack->top == 0, 0))
    return (u64) -1;

  return stack->addrs[--stack->top];
}
