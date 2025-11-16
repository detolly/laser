#pragma once

#ifdef _POSIX_C_SOURCE
#error include laser/debug.h before standard libraries.
#endif

#define _POSIX_C_SOURCE 200809L

#ifdef LASER_DEBUG

#include <stdio.h>
#include <stdint.h>
#include <time.h>

static inline uint64_t ns_now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (unsigned long long)ts.tv_nsec;
}

#define DEBUG(x, ...) fprintf(stderr, x, __VA_ARGS__)

#else

#define DEBUG(x, ...)

#endif
