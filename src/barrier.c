/*   
 *   File: barrier.c
 *   Author: Vasileios Trigonakis <vasileios.trigonakis@epfl.ch>
 *   Description: implementation of process barriers
 *   barrier.c is part of ccbench
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

#include "barrier.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sched.h>
#include <inttypes.h>

#ifdef __sparc__
#  include <sys/types.h>
#  include <sys/processor.h>
#  include <sys/procset.h>
#endif	/* __sparc__ */

barrier_t* barriers;


int color_all(int id)
{
  return 1;
}

void
barriers_init(const uint32_t num_procs)
{
  uint32_t size;
  size = NUM_BARRIERS * sizeof(barrier_t);
  if (size < 8192)
    {
      size = 8192;
    }

  char keyF[100];
  sprintf(keyF, BARRIER_MEM_FILE);

  int barrierfd = shm_open(keyF, O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG);
  if (barrierfd<0)
    {
      if (errno != EEXIST)
	{
	  perror("In shm_open");
	  exit(1);
	}

      //this time it is ok if it already exists
      barrierfd = shm_open(keyF, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
      if (barrierfd<0)
	{
	  perror("In shm_open");
	  exit(1);
	}
    }
  else
    {
      if (ftruncate(barrierfd, size) < 0) {
	perror("ftruncate failed\n");
	exit(1);
      }
    }

  void* mem = (void*) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, barrierfd, 0);
  if (mem == NULL)
    {
      perror("ssmp_mem = NULL\n");
      exit(134);
    }

  barriers = (barrier_t*) mem;

  uint32_t bar;
  for (bar = 0; bar < NUM_BARRIERS; bar++) 
    {
      barrier_init(bar, 0, color_all, num_procs);
    }
}

void
barrier_init(const uint32_t barrier_num, const uint64_t participants, int (*color)(int),
	     const uint32_t total_cores) 
{
  if (barrier_num >= NUM_BARRIERS) 
    {
      return;
    }


  barriers[barrier_num].num_crossing1 = 0;
  barriers[barrier_num].num_crossing2 = 0;
  barriers[barrier_num].num_crossing3 = 0;
  barriers[barrier_num].color = color;
  uint32_t ue, num_parts = 0;
  for (ue = 0; ue < total_cores; ue++) 
    {
      num_parts += color(ue);
    }
  barriers[barrier_num].num_participants = num_parts;

}


void 
barrier_wait(const uint32_t barrier_num, const uint32_t id, const uint32_t total_cores) 
{
  _mm_mfence();
  if (barrier_num >= NUM_BARRIERS) 
    {
      return;
    }

  //  printf("enter: %d : %d\n", barrier_num, id);

  barrier_t *b = &barriers[barrier_num];

  int (*col)(int);
  col = b->color;

  if (col(id) == 0) 
    {
      return;
    }


  b->num_crossing2 = 0;
  FAI_U64(&b->num_crossing1);

  while (b->num_crossing1 < b->num_participants)
    {
      PAUSE();
      _mm_mfence();
    }


  b->num_crossing3 = 0;

  FAI_U64(&b->num_crossing2);

  while (b->num_crossing2 < b->num_participants)
    {
      PAUSE();
      _mm_mfence();
    }

  b->num_crossing1 = 0;

  FAI_U64(&b->num_crossing3);

  while (b->num_crossing3 < b->num_participants)
    {
      PAUSE();
      _mm_mfence();
    }

  //  printf("EXIT : %d : %d\n", barrier_num, id);

}

void
barriers_term(const uint32_t id) 
{
  if (id == 0)
    {
      shm_unlink(BARRIER_MEM_FILE);
    }
}
