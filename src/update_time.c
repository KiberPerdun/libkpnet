  //
  // Created by KiberPerdun on 12/19/25.
  //

  #include "if_packet.h"

  u64
  update_time (u0)
  {
    struct timespec ts;
    clock_gettime (CLOCK_MONOTONIC, &ts);
    return (u64) ts.tv_sec * 1000ULL + (u64) ts.tv_nsec / 1000000ULL;
  }
