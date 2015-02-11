/*   
 *   File: ccbench.h
 *   Author: Vasileios Trigonakis <vasileios.trigonakis@epfl.ch>
 *   Description: definition of ccbench events and help functions
 *   ccbench.h is part of ccbench
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

#ifndef _H_CCBENCH_
#define _H_CCBENCH_

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <assert.h>
#include <float.h>
#include <getopt.h>

#if defined(__amd64__)
#  include <emmintrin.h>
#elif defined(__tile__)
#  include <tmc/mem.h>
#  include <tmc/cmem.h>
#  include <tmc/cpus.h>
extern cpu_set_t cpus;
#endif	

#if defined(PLATFORM_NUMA)
#  include <numa.h>
#endif	/* PLATFORM_NUMA */

#include "common.h"
#include "pfd.h"
#include "barrier.h"

typedef struct cache_line
{
  volatile uint32_t word[16];
} cache_line_t;

#define CACHE_LINE_NUM      1024*1024 /* power of 2 pls */
#define CACHE_LINE_STRIDE_2 2047

# define LLU unsigned long long int

extern volatile cache_line_t* cache_line_open();
extern void cache_line_close(const uint32_t id, const char* name);

typedef enum
  {
    STORE_ON_MODIFIED,
    STORE_ON_MODIFIED_NO_SYNC,
    STORE_ON_EXCLUSIVE,
    STORE_ON_SHARED,
    STORE_ON_OWNED_MINE,
    STORE_ON_OWNED,
    STORE_ON_INVALID,
    LOAD_FROM_MODIFIED,
    LOAD_FROM_EXCLUSIVE,
    LOAD_FROM_SHARED,
    LOAD_FROM_OWNED,
    LOAD_FROM_INVALID,
    CAS,
    FAI,
    TAS,
    SWAP,
    CAS_ON_MODIFIED,
    FAI_ON_MODIFIED,
    TAS_ON_MODIFIED,
    SWAP_ON_MODIFIED,
    CAS_ON_SHARED,
    FAI_ON_SHARED,
    TAS_ON_SHARED,
    SWAP_ON_SHARED,
    CAS_CONCURRENT,
    FAI_ON_INVALID,
    LOAD_FROM_L1,
    LOAD_FROM_MEM_SIZE,
    LFENCE,
    SFENCE,
    MFENCE,
    PROFILER,
    PAUSE,
    NOP,
    NUM_EVENTS,			/* placeholder for printing the num of events */
  } moesi_type_t;

const char* moesi_type_des[] =
  {
    "STORE_ON_MODIFIED",
    "STORE_ON_MODIFIED_NO_SYNC",
    "STORE_ON_EXCLUSIVE",
    "STORE_ON_SHARED",
    "STORE_ON_OWNED_MINE",
    "STORE_ON_OWNED",
    "STORE_ON_INVALID",
    "LOAD_FROM_MODIFIED",
    "LOAD_FROM_EXCLUSIVE",
    "LOAD_FROM_SHARED",
    "LOAD_FROM_OWNED",
    "LOAD_FROM_INVALID",
    "CAS",
    "FAI",
    "TAS",
    "SWAP",
    "CAS_ON_MODIFIED",
    "FAI_ON_MODIFIED",
    "TAS_ON_MODIFIED",
    "SWAP_ON_MODIFIED",
    "CAS_ON_SHARED",
    "FAI_ON_SHARED",
    "TAS_ON_SHARED",
    "SWAP_ON_SHARED",
    "CAS_CONCURRENT",
    "FAI_ON_INVALID",
    "LOAD_FROM_L1",
    "LOAD_FROM_MEM_SIZE",
    "LFENCE",
    "SFENCE",
    "MFENCE",
    "PROFILER",
    "PAUSE",
    "NOP",
  };


#define DEFAULT_CORES       2
#define DEFAULT_REPS        10000
#define DEFAULT_TEST        0
#define DEFAULT_CORE1       0
#define DEFAULT_CORE2       1
#define DEFAULT_CORE3       2
#define DEFAULT_CORE_OTHERS 0
#define DEFAULT_FLUSH       0
#define DEFAULT_VERBOSE     0
#define DEFAULT_PRINT       100
#define DEFAULT_STRIDE      (CACHE_LINE_STRIDE_2 + 1)
#define DEFAULT_FENCE       0
#define DEFAULT_LFENCE       0
#define DEFAULT_SFENCE       0
#define DEFAULT_AO_SUCCESS  0


#define CACHE_LINE_MEM_FILE "/cache_line"

#define B0 _mm_mfence(); barrier_wait(0, ID, test_cores); _mm_mfence();
#define B1 _mm_mfence(); barrier_wait(2, ID, test_cores); _mm_mfence();
#define B2 _mm_mfence(); barrier_wait(3, ID, test_cores); _mm_mfence();
#define B3 _mm_mfence(); barrier_wait(4, ID, test_cores); _mm_mfence();
#define B4 _mm_mfence(); barrier_wait(5, ID, test_cores); _mm_mfence();
#define B5 _mm_mfence(); barrier_wait(6, ID, test_cores); _mm_mfence();
#define B6 _mm_mfence(); barrier_wait(7, ID, test_cores); _mm_mfence();
#define B7 _mm_mfence(); barrier_wait(8, ID, test_cores); _mm_mfence();
#define B8 _mm_mfence(); barrier_wait(9, ID, test_cores); _mm_mfence();
#define B9 _mm_mfence(); barrier_wait(10, ID, test_cores); _mm_mfence();
#define B10 _mm_mfence(); barrier_wait(11, ID, test_cores); _mm_mfence();
#define B11 _mm_mfence(); barrier_wait(12, ID, test_cores); _mm_mfence();
#define B12 _mm_mfence(); barrier_wait(13, ID, test_cores); _mm_mfence();
#define B13 _mm_mfence(); barrier_wait(14, ID, test_cores); _mm_mfence();
#define B14 _mm_mfence(); barrier_wait(15, ID, test_cores); _mm_mfence();

#define XSTR(s)                         STR(s)
#define STR(s)                          #s

#ifndef ALIGNED
#  if __GNUC__ && !SCC
#    define ALIGNED(N) __attribute__ ((aligned (N)))
#  else
#    define ALIGNED(N)
#  endif
#endif

inline void
set_cpu(int cpu) 
{
#if defined(__sparc__)
  processor_bind(P_LWPID,P_MYID, cpu, NULL);
#elif defined(__tile__)
  if (tmc_cpus_set_my_cpu(tmc_cpus_find_nth_cpu(&cpus, cpu)) < 0)
    {
      tmc_task_die("Failure in 'tmc_cpus_set_my_cpu()'.");
    }

  if (cpu != tmc_cpus_get_my_cpu())
    {
      PRINT("******* i am not CPU %d", tmc_cpus_get_my_cpu());
    }

#else
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(cpu, &mask);
  if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) != 0) {
    printf("Problem with setting processor affinity: %s\n",
	   strerror(errno));
    exit(3);
  }
#endif

#ifdef OPTERON
  uint32_t numa_node = cpu/6;
  numa_set_preferred(numa_node);  
#elif defined(XEON)
  uint32_t numa_node = 0;
  if (cpu == 0)
    {
      numa_node = 4;
    }
  else if (cpu <= 40)
    {
      numa_node = (cpu - 1) / 10;
    }
  else
    {
      numa_node = cpu / 10;
    }
  numa_set_preferred(numa_node);  
#elif defined(PLATFORM_NUMA)
  printf("* You need to define how cores correspond to mem nodes in ccbench.h\n");
#endif 
  
}

inline void 
wait_cycles(volatile uint64_t cycles)
{
  /* cycles >>= 1; */
  for (cycles; cycles > 0; cycles--)
    {
      asm volatile ("nop");
    }
}

  /* getticks needs to have a correction because the call itself takes a */
  /* significant number of cycles and skewes the measurement */
static inline ticks getticks_correction_calc() 
{
#define GETTICKS_CALC_REPS 1000000
  ticks t_dur = 0;
  uint32_t i;
  for (i = 0; i < GETTICKS_CALC_REPS; i++) 
    {
      ticks t_start = getticks();
      ticks t_end = getticks();
      t_dur += t_end - t_start;
    }
  //    printf("corr in float %f\n", (t_dur / (double) GETTICKS_CALC_REPS));
  ticks getticks_correction = (ticks)(t_dur / (double) GETTICKS_CALC_REPS);
  return getticks_correction;
}

#define IN_ORDER(id, num_cores)			\
  {						\
    B0;						\
    uint32_t c;					\
    for (c = 0; c < num_cores; c++)		\
      {						\
	if (id == c)				\
	  {					

#define IN_ORDER_END				\
	  }					\
	B0;					\
      }						\
  }


  static inline unsigned long* 
  seed_rand() 
  {
    unsigned long* seeds;
    seeds = (unsigned long*) malloc(3 * sizeof(unsigned long));
    seeds[0] = getticks() % 123456789;
    seeds[1] = getticks() % 362436069;
    seeds[2] = getticks() % 521288629;
    return seeds;
  }

extern unsigned long* seeds;
  //Marsaglia's xorshf generator //period 2^96-1
static inline unsigned long
xorshf96(unsigned long* x, unsigned long* y, unsigned long* z) 
{          
  unsigned long t;
  (*x) ^= (*x) << 16;
  (*x) ^= (*x) >> 5;
  (*x) ^= (*x) << 1;

  t = *x;
  (*x) = *y;
  (*y) = *z;
  (*z) = t ^ (*x) ^ (*y);

  return *z;
}
#define clrand() (xorshf96(seeds, seeds + 1, seeds + 2) & (test_stride - 1))
#define sirand(range) ((xorshf96(seeds, seeds + 1, seeds + 2) % range) + 64)
#define my_random(a, b, c) xorshf96(a, b, c)

static inline uint32_t pow2roundup (uint32_t x)
{
  if (x==0) return 1;
  --x;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  return x+1;
}
#endif	/* _H_CCBENCH_ */
