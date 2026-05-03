//
// Created by KiberPerdun on 03.05.2026.
//

#include "if_packet.h"

kpnet_env_t *
kpnet_env_init (const char *ifname)
{
  kpnet_env_t *env = malloc (sizeof (kpnet_env_t));
  if (!env) return NULL;

  env->stack = create_umem_stack (UMEM_CHUNK_COUNT, UMEM_CHUNK_SIZE);
  if (!env->stack)
    goto err_stack;

  env->xdp = xdp_open (ifname);
  if (!env->xdp)
    goto err_xdp;

  env->eth = eth_open (ifname);
  if (!env->eth)
    goto err_eth;

  env->state_alloc.free_list = NULL;
  env->state_alloc.cur_pos = NULL;
  env->state_alloc.cur_end = NULL;
  env->state_alloc.stack = env->stack;

  return env;

  err_eth:
    /* TODO! xdp_close */
  err_xdp:
    free (env->stack);
  err_stack:
    free (env);
  return NULL;
}
