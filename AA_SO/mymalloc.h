#ifndef __MALLOC_H__
#define __MALLOC_H__

#define DEBUG 0
#define debug_print(args ...) if (DEBUG) fprintf(stderr, args)
#define MIN_MALLOC 4096
#define THRESHOLD 16
#define MALLOC_MAGIC 0xDEADBEEF
#define FREE_MAGIC 0x42424242

typedef struct s_header
{
  unsigned int magic;
  int size;
  struct s_header *prox;
}t_header;

extern t_header	*gl_freelist;
extern void	*gl_lowlimit;


void *myMalloc(int size);
void myFree(void *address);
void *findFreeBlock(int size);
void addToFreeList(t_header *tmp, int fusion);
void deleteFromFreeList(t_header *tmp);
int getListNb(int size);
void split_mid(t_header *tmp, int total_size);
void split_end(t_header *tmp);
//void show_alloc_mem(void);
void verifyFusion(t_header *begin);
t_header *verifyPtr(void *ptr);

#endif /* __MALLOC_H__ */
