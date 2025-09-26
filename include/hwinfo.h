//
// Created by KiberPerdun on 9/23/25.
//

#ifndef LIBKPNET_HWINFO_H
#define LIBKPNET_HWINFO_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "if_packet.h"

#ifdef _SC_LEVEL1_DCACHE_LINESIZE
#define CACHELINE_SIZE sysconf(_SC_LEVEL1_DCACHE_LINESIZE)
#else
#define CACHELINE_SIZE 64
#endif

#endif // LIBKPNET_HWINFO_H
