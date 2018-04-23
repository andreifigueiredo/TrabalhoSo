/*****************************************
*gcc -g -c mymalloc.c 
*****************************************/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include "mymalloc.h"

/****************************************************************************
*freeList
*****************************************************************************/
t_header	*gl_freelist = NULL;
/****************************************************************************
*memoryList
*****************************************************************************/
void *gl_lowlimit = NULL;
sem_t S ;

/****************************************************************************
*Funcao que procura ponteiro dentro da memoryList
*****************************************************************************/

t_header *verifyPtr(void *ptr){
  t_header  *tmp;

  if (ptr == NULL)
    return (NULL);
  sem_wait(&S);
  if (gl_lowlimit == NULL){
      debug_print("[malloc] Erro: Nao foi encontrado o primeiro malloc\n");
      unlock_thread();
      return (NULL);
    }
  if (ptr < gl_lowlimit){
      debug_print("[malloc] Erro: Address está antes do low limit.\n");
      unlock_thread();
      return (NULL);
    }
  tmp = (t_header *)((int)ptr - sizeof(*tmp));
  sem_post (&S);
  return (tmp);
}

/****************************************************************************
*Funcao que insere bloco na freeList ordenado
*****************************************************************************/

static void insertInOrder(t_header *last, t_header *tmp, int fusion){
  t_header  *current;

  current = last->prox;
  while (current){
      if (tmp > current){
        last = current;
        current = current->prox;
      }
      else
        break;
  }
  last->prox = tmp;
  tmp->prox = current;
  if (fusion == 1)
    verifyFusion(last);
}

/****************************************************************************
*Funcao que remove bloco da FreeList
*****************************************************************************/

void deleteFromFreeList(t_header *tmp){
  t_header  *begin;
  t_header  *save;
/****************************************************************************
*Funcao que verifica se é possivel unir dois blocos
*****************************************************************************/
  begin = gl_freelist;
  if (tmp == gl_freelist)
    gl_freelist = gl_freelist->prox;
  else{
      save = gl_freelist;
      while (save->prox != tmp)
        save = save->prox;
      save->prox = tmp->prox;
    }
  if (tmp){
      tmp->magic = MALLOC_MAGIC;
      tmp->prox = NULL;
    }
}

/****************************************************************************
*Funcao que insere bloco na FreeList
*****************************************************************************/

void addToFreeList(t_header *tmp, int fusion){
  t_header  *last;

  tmp->magic = FREE_MAGIC;
  tmp->prox = NULL;
  last = gl_freelist;
  if (last == NULL)
    gl_freelist = tmp;
  else{
    if (tmp < last){
      tmp->prox = gl_freelist;
      gl_freelist = tmp;
     }
     else
      insertInOrder(last, tmp, fusion);
    verifyFusion(tmp);
    }
}

void split_mid(t_header *tmp, int total_size){
  t_header  *newBlock;
  int offset;

  offset = sizeof(*tmp) + tmp->size;
  newBlock = (t_header *)((int)tmp + offset);
  newBlock->magic = FREE_MAGIC;
  newBlock->size = total_size - offset - sizeof(*newBlock);
  newBlock->prox = tmp->prox;
  tmp->prox = newBlock;
  verifyFusion(newBlock);
}

void split_end(t_header *tmp){
  t_header  *newBlock;
  int   offset;

  offset = sizeof(*tmp) + tmp->size;
  newBlock = (t_header *)((int)tmp + offset);
  newBlock->size = MIN_MALLOC - offset - sizeof(*newBlock);
  addToFreeList(newBlock, 0);
}

static void	*myMallocInit(int size){
  int		units;
  void		*address;
  t_header	*tmp;

  units = size;
  if (units < MIN_MALLOC)
    units = MIN_MALLOC;
  address = sbrk(units);
  if (address == (void *)-1)
    return (NULL);
  tmp = address;
  tmp->magic = MALLOC_MAGIC;
  tmp->prox = NULL;
  if (MIN_MALLOC - size >= THRESHOLD){
      tmp->size = size - sizeof(*tmp);
      split_end(tmp);
    }
  else
    tmp->size = size - sizeof(*tmp);
  if (gl_lowlimit == NULL)
    gl_lowlimit = tmp;
  return (tmp);
}

void *myMalloc(int size){
  t_header	*tmp;
  int		total_size;

  if (size <= 0)
    return (NULL);
  sem_wait(&S);
  if ((size % sizeof(int)) != 0)
    size += (sizeof(int) - (size % sizeof(int)));
  if ((tmp = (t_header *)findFreeBlock(size)) == NULL)
    tmp = (t_header *)myMallocInit(size + sizeof(*tmp));
  else{
      if (tmp->size - size >= THRESHOLD){
    	  total_size = tmp->size + sizeof(*tmp);
    	  tmp->size = size;
    	  split_mid(tmp, total_size);
	}
      deleteFromFreeList(tmp);
    }
  sem_post(&S);
  if (tmp == NULL)
    return (NULL);
  return ((void *)((int)tmp + sizeof(*tmp)));
}

/****************************************************************************
*Funcao que verifica se é possivel unir dois blocos
*****************************************************************************/

void verifyFusion(t_header *begin){
  t_header  *block;

  if (begin && begin->prox){
    block = (t_header *)((int)begin + begin->size + sizeof(*begin));
    if (block->magic == FREE_MAGIC){
      begin->size = begin->size + block->size + sizeof(*block);
      begin->prox = block->prox;
    }
  }
}

/****************************************************************************
*Funcao que busca bloco livre na lista
*****************************************************************************/

void *findFreeBlock(int size){
  t_header  *begin;
  t_header  *tmp;
  unsigned int  min_size;

  tmp = NULL;
  min_size = -1;
  begin = gl_freelist;
  while (begin){
      if ((unsigned int)begin->size < min_size && begin->size >= size){
        tmp = begin;
        min_size = begin->size;
      }
      begin = begin->prox;
    }
  return (tmp);
}

void myFree(void *addr){
  t_header  *tmp;

  tmp = verifyPtr(addr);
  if (tmp == NULL)
    return ;
  sem_wait(&S);
  if (tmp->magic == FREE_MAGIC)
    debug_print("[malloc] Erro: O block ja ta free.\n");
  if (tmp->magic == MALLOC_MAGIC)
    addToFreeList(tmp, 1);
  else
    debug_print("[malloc] Erro: Junk Pointer.\n");
  sem_post(&S);
}

/****************************************************************************
*Funcao que mostra memoria alocada(Não funciona)
*****************************************************************************

void show_alloc_mem(){
  void    *addr;
  t_header  *tmp;
  int   offset;
  int   size;

  addr = gl_lowlimit;
  if (addr == NULL)
    return ;
  while ((int)addr <= (int)end){
    tmp = addr;
    offset = sizeof(*tmp) + tmp->size + (int)tmp;
    size = tmp->size + sizeof(*tmp);
    if (tmp->magic == MALLOC_MAGIC)
      debug_print("0x%x - 0x%d : %d octetos\n", tmp, offset, size);
    addr = (void *)((int)addr + tmp->size + sizeof(*tmp));
  }
}*/