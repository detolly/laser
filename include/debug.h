#pragma once


#ifdef LASER_DEBUG

#define DEBUG(x, ...) fprintf(stderr, x, __VA_ARGS__)

#else

#define DEBUG(x, ...)

#endif
