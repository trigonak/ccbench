/*   
 *   File: pfd.h
 *   Author: Vasileios Trigonakis <vasileios.trigonakis@epfl.ch>
 *   Description: pfd interface, structures, and helper functions
 *   pfd.h is part of ccbench
 *
 * The MIT License (MIT)
 *
 * Copyright (C) 2013  Vasileios Trigonakis
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _PFD_H_
#define _PFD_H_

#include <inttypes.h>
#include <float.h>
#include <assert.h>
#include <stdlib.h>
#include "common.h"


typedef uint64_t ticks;

#if defined(__i386__)
static inline ticks 
getticks(void) 
{
  ticks ret;

  __asm__ __volatile__("rdtsc" : "=A" (ret));
  return ret;
}
#elif defined(__x86_64__)
static inline ticks
getticks(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}
#elif defined(__sparc__)
static inline ticks
getticks()
{
  ticks ret;
  __asm__ __volatile__ ("rd %%tick, %0" : "=r" (ret) : "0" (ret)); 
  return ret;
}
#elif defined(__tile__)
#include <arch/cycle.h>
static inline ticks getticks()
{
  return get_cycle_count();
}
#endif


#define DO_TIMINGS

#if !defined(PREFETCHW)
#  if defined(__x86_64__) | defined(__i386__)
#    define PREFETCHW(x) asm volatile("prefetchw %0" :: "m" (*(unsigned long *)x)) /* write */
#  elif defined(__sparc__)
#    define PREFETCHW(x) __builtin_prefetch((const void*) x, 1, 3)
#  elif defined(__tile__)
#    define PREFETCHW(x) tmc_mem_prefetch (x, 64)
#  else
#    warning "You need to define PREFETCHW(x) for your architecture"
#  endif
#endif

typedef struct abs_deviation
{
  uint64_t num_vals;
  double avg;
  double avg_10p;
  double avg_25p;
  double avg_50p;
  double avg_75p;
  double avg_rst;
  double abs_dev_10p;
  double abs_dev_25p;
  double abs_dev_50p;
  double abs_dev_75p;
  double abs_dev_rst;
  double abs_dev;
  double std_dev_10p;
  double std_dev_25p;
  double std_dev_50p;
  double std_dev_75p;
  double std_dev_rst;
  double std_dev;
  double min_val;
  uint64_t min_val_idx;
  double max_val;
  uint64_t max_val_idx;
  uint32_t num_dev_10p;
  uint32_t num_dev_25p;
  uint32_t num_dev_50p;
  uint32_t num_dev_75p;
  uint32_t num_dev_rst;
} abs_deviation_t;


#define PFD_NUM_STORES 2
#define PFD_PRINT_MAX 200

extern volatile ticks** pfd_store;
extern volatile ticks* _pfd_s;
extern volatile ticks pfd_correction;
#if !defined(DO_TIMINGS)
#  define PFDINIT(num_entries) 
#  define PFDI(store) 
#  define PFDO(store, entry) 
#  define PFDP(store, num_vals) 
#  define PFDPN(store, num_vals, num_print)
#else  /* DO_TIMINGS */
#  define PFDINIT(num_entries) pfd_store_init(num_entries)

#  define PFDI(store)				\
  {						\
  asm volatile ("");				\
  _pfd_s[store] = getticks();


#  define PFDO(store, entry)						\
  asm volatile ("");							\
  pfd_store[store][entry] =  getticks() - _pfd_s[store] - pfd_correction; \
  }

#  define PFDOR(store, entry, reps)					\
  asm volatile ("");							\
  volatile ticks __t = getticks();					\
  pfd_store[store][entry] = (__t - _pfd_s[store] - pfd_correction) /	\
    reps;								\
  }

#  define PFDPN(store, num_vals, num_print)				\
  {									\
    uint32_t _i;							\
    uint32_t p = num_print;						\
    if (p > num_vals) { p = num_vals; }					\
    for (_i = 0; _i < p; _i++)						\
      {									\
	printf("[%3d: %4ld] ", _i, (long int) pfd_store[store][_i]);	\
      }									\
    abs_deviation_t ad;							\
    get_abs_deviation(pfd_store[store], num_vals, &ad);			\
    print_abs_deviation(&ad);						\
  }
#endif /* !DO_TIMINGS */

# define PFDPREFTCH(store, entry)		\
  PFDI(store);					\
  PFDO(store, entry);



void pfd_store_init(const uint32_t num_entries);
void get_abs_deviation(volatile ticks* vals, const size_t num_vals, abs_deviation_t* abs_dev);
void print_abs_deviation(const abs_deviation_t* abs_dev);


#endif	/* _PFD_H_ */
