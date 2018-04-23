#include <unistd.h>
#include "malloc.h"

t_header	*gl_freelist = NULL;
void		*gl_lowlimit = NULL;
int		thread_locked = 0;

int	*myMallocInit(int size)
{
  int		units;
  void		*address;
  t_header	*tmp;

  units = size;
  if (units < MIN_MALLOC)
    units = MIN_MALLOC;
  address = sbrk(units);
  if (address == (void *)-1)
    return 1;
  tmp = address;
  tmp->magic = MALLOC_MAGIC;
  tmp->next = NULL;
  if (MIN_MALLOC - size >= THRESHOLD)
    {
      tmp->size = size - sizeof(*tmp);
      split_end(tmp);
    }
  else
    tmp->size = size - sizeof(*tmp);
  if (gl_lowlimit == NULL)
    gl_lowlimit = tmp;
  return 0;
}

void	*myMalloc(int size)
{
  t_header	*tmp;
  int		total_size;

  if (size <= 0)
    return (NULL);
  lock_thread();
  if ((size % sizeof(int)) != 0)
    size +
= (sizeof(int) - (size % sizeof(int)));
  if ((tmp = (t_header *)findFreeBlock(size)) == NULL)
    tmp = (t_header *)myMallocInit(size + sizeof(*tmp));
  else
    {
      if (tmp->size - size >= THRESHOLD)
	{
	  total_size = tmp->size + sizeof(*tmp);
	  tmp->size = size;
	  split_mid(tmp, total_size);
	}
      deleteFromFreeList(tmp);
    }
  unlock_thread();
  if (tmp == NULL)
    return (NULL);
  return ((void *)((int)tmp + sizeof(*tmp)));
}

void verifyFusion(t_header *begin)
{
  t_header  *block;

  if (begin && begin->next)
    {
      block = (t_header *)((int)begin + begin->size + sizeof(*begin));
      if (block->magic == FREE_MAGIC)
  {
    begin->size = begin->size + block->size + sizeof(*block);
    begin->next = block->next;
  }
    }
}

void *findFreeBlock(int size)
{
  t_header  *begin;
  t_header  *tmp;
  unsigned int  min_size;

  tmp = NULL;
  min_size = -1;
  begin = gl_freelist;
  while (begin)
    {
      if ((unsigned int)begin->size < min_size && begin->size >= size)
  {
    tmp = begin;
    min_size = begin->size;
  }
      begin = begin->next;
    }
  return (tmp);
}

void myFree(void *addr)
{
  t_header  *tmp;

  tmp = verifyPtr(addr);
  if (tmp == NULL)
    return ;
  lock_thread();
  if (tmp->magic == FREE_MAGIC)
    my_fprintf(2, "[malloc] Error: The block is already free.\n");
  if (tmp->magic == MALLOC_MAGIC)
    addToFreeList(tmp, 1);
  else
    my_fprintf(2, "[malloc] Error: Junk Pointer.\n");
  unlock_thread();
}