/*****************************************
*gcc -Wall mallocteste.c -o mallocteste -lpthread 
*****************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <pthread.h>
#include "App.h"

#define DEBUG 1
#define debug_print(args ...) if (DEBUG) fprintf(stderr, args)

pid_t gettid(void);
void *membench(void *arg);


// Parametos das threads
typedef struct {
  int mallocSize;
  int count;
  int loop;
} THREAD_ARGS;


// main
int main(int argc, char **argv){

  int c;
  int threadNum = 1;

  Stopwatch sw;

  THREAD_ARGS thread_args;
  thread_args.mallocSize = 1;
  thread_args.count = 1;
  thread_args.loop = 1;

  static struct option long_options[] =
  {
    {"threads", required_argument, 0, 't'},
    {"size", required_argument, 0, 's'},
    {"count", required_argument, 0, 'c'},
    {"loop", required_argument, 0, 'l'},
    {0, 0, 0, 0}
  };

  int option_index = 0;

  while((c = getopt_long( argc, argv, "t:s:c:l:v", long_options, &option_index)) != -1) {

    switch(c) {
      case 't':
        threadNum = atoi(optarg);
        break;
      case 's':
        thread_args.mallocSize = atoi(optarg);
        break;
      case 'c':
        thread_args.count = atoi(optarg);
        break;
      case 'l':
        thread_args.loop = atoi(optarg);
        break;
      case ':':
      case '?':
        fprintf(stderr, "Usage: %s --thread=THREADNUmBER --size=mALLOCSIZE --count=mALLOCCOUNT --loop=LOOPCOUNT\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  int i;
  pthread_t p[threadNum];

  printf("Number of threads                      : %d\n", threadNum);
  printf("malloc size                            : %d mB\n", thread_args.mallocSize);
  printf("Number of councurrent malloc per thread: %d\n", thread_args.count);
  printf("Number of loops per thread             : %d\n", thread_args.loop);

  // Dispara cronometro
  START_STOPWATCH(sw);

  // Create Threads
  for( i = 0; i < threadNum; i++) {
    pthread_create(&p[i], NULL, membench, &thread_args);
  }

  // Closed Threads
  for( i = 0; i < threadNum; i++) {
    pthread_join(p[i], NULL);
  }

  // Para cronometro
  STOP_STOPWATCH(sw);

  printf("%f milisegundos\n", sw.mElapsedTime);

  exit(EXIT_SUCCESS);
}

// Get Thread Id
pid_t gettid(void) {
  return syscall(SYS_gettid);
}

// memory Benchmark
void *membench(void *arg) {

  THREAD_ARGS *thread_args = (THREAD_ARGS*)arg;

  int mallocSize = thread_args->mallocSize;
  int count      = thread_args->count;
  int loop       = thread_args->loop;

  int *mem[count];
  int size[count];

  int i, loopcnt = 0;

  while (1) {

    if ( loop != 0 && ++loopcnt > loop) {
      break;
    }

    // malloc()
    for(i = 0; i < count; i++) {
      size[i] = (rand() % (mallocSize * 10)  + 1) * 100 * 1000;
      mem[i]  = (int *)malloc(size[i]);

      if (!mem[i]) {
        puts("can't malloc");
        return NULL;
      }
      memset(mem[i], 1, size[i]);
      memset(mem[i], 0, size[i]);
      debug_print("thread: %d, loop: %d, mem[%d] %d Bytes malloc\n", gettid(), loopcnt, i, size[i]);
    }

    // free()
    for(i = 0; i < count; i++) {
      free(mem[i]);
        debug_print("thread: %d, loop: %d, mem[%d] %d Bytes free\n", gettid(), loopcnt, i, size[i]);
    }
  }
  return NULL;
}
