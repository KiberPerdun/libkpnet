//
// Created by KiberPerdun on 25.10.24.
//
#include "tcp.h"

u16
tcp_checksum (u16 *ptr, int nbytes) {
  u32 sum = 0;

  while (nbytes > 1) {
      sum += *ptr++;
      nbytes -= 2;
    }

  if (nbytes == 1) {
      u16 last_byte;

      last_byte = 0;
      *((u8 *) &last_byte) = *(u8 *) ptr;
      sum += last_byte;
    }

  while (sum >> 16)
    sum = (sum & 0xFFFF) + (sum >> 16);

  return ~sum;
}
