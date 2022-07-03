//
// Created by pedro on 24/06/22.
//

#ifndef MERGESORT_TIMER_H
#define MERGESORT_TIMER_H

#include <sys/time.h>
#define BILLION 1000000000L

/* The argument now should be a double (not a pointer to a double) */
#define GET_TIME(now) { \
   struct timespec time; \
   clock_gettime(CLOCK_MONOTONIC, &time); \
   now = time.tv_sec + time.tv_nsec/1000000000.0; \
}

#endif //MERGESORT_TIMER_H
