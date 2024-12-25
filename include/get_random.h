//
// Created by KiberPerdun on 15.12.2024.
//

#ifndef LIBKPNET_GET_RANDOM_H
#define LIBKPNET_GET_RANDOM_H

#include "types.h"
#include <sys/random.h>
#include <stdio.h>

u64 get_random_u64 ();
u32 get_random_u32 ();
u16 get_random_u16 ();
u8 get_random_u8 ();
u0 get_random (u0 *buf, u64 size);

#endif // LIBKPNET_GET_RANDOM_H
