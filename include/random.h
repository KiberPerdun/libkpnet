//
// Created by KiberPerdun on 15.12.2024.
//

#ifndef LIBKPNET_RANDOM_H
#define LIBKPNET_RANDOM_H

#include "types.h"
#include <sys/random.h>
#include <stdio.h>

#define RANDOM_BUFFER_SIZE 1024

extern u8 random_buffer[RANDOM_BUFFER_SIZE];
extern _Atomic u32 random_buffer_head;

u64 get_random_u64 ();
u32 get_random_u32 ();
u16 get_random_u16 ();
u8 get_random_u8 ();
u0 get_random (u0 *buf, u64 size);

u0 generate_random_buffer ();
u64 get_random_from_buffer_u64 ();
u32 get_random_from_buffer_u32 ();
u16 get_random_from_buffer_u16 ();
u8 get_random_from_buffer_u8 ();

#endif // LIBKPNET_RANDOM_H
