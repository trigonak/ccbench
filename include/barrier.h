/*   
 *   File: barrier.h
 *   Author: Vasileios Trigonakis <vasileios.trigonakis@epfl.ch>
 *   Description: barrier structures
 *   barrier.h is part of ccbench
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

#ifndef BARRIER_H
#define	BARRIER_H

#include "common.h"
#include "atomic_ops.h"
#ifdef __sparc__
#  include <sys/types.h>
#  include <sys/processor.h>
#  include <sys/procset.h>
#endif /* __sparc */

#define NUM_BARRIERS 16
#define BARRIER_MEM_FILE "/barrier_mem"

#ifndef ALIGNED
#  if __GNUC__ && !SCC
#    define ALIGNED(N) __attribute__ ((aligned (N)))
#  else
#    define ALIGNED(N)
#  endif
#endif

/*barrier type*/
typedef ALIGNED(64) struct barrier
{
  uint64_t num_participants;
  volatile uint64_t num_crossing1;
  volatile uint64_t num_crossing2;
  volatile uint64_t num_crossing3;
  int (*color)(int); /*or color function: if return 0 -> no , 1 -> participant. Priority on this */
} barrier_t;


void barriers_init(const uint32_t num_procs);
void barrier_init(const uint32_t barrier_num, const uint64_t participants, int (*color)(int), const uint32_t);
void barrier_wait(const uint32_t barrier_num, const uint32_t id, const uint32_t total_cores);
void barriers_term();

#ifdef __sparc__
#  define PAUSE()    asm volatile("rd    %%ccr, %%g0\n\t"	\
				::: "memory")
#elif defined(__tile__)
#define PAUSE() cycle_relax()
#else
#define PAUSE() _mm_pause()
#endif

#endif	/* BARRIER_H */
