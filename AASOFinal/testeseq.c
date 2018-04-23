/****************************************************
*gcc -g -c testeseq.c 
*gcc -g -o testeseq testeseq.c mymalloc.o
****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "App.h"
#include "mymalloc.h"

int main(int argc, char *argv[])
{
  Stopwatch sw;

  debug_print("Block header size is %ld\n",sizeof(Block));
  
  // Dispara cronometro
  START_STOPWATCH(sw);

  //test that request for 0 returns null
  char *fail = (char *)myMalloc(0);
  if(fail != NULL) {
    printf("Test failed: request to allocate 0 bytes should return null.\n");
  }
  
  myFree(fail); // should have no effect -- pointer is null
  
  
  //test some simple allocations/frees
  
  char *r = (char *)myMalloc(10000*sizeof(char));
  memoryMap();
  
  double *q = (double *)myMalloc(50000*sizeof(double));
  memoryMap();
  
  myFree(r);
  memoryMap();
  
  myFree(r); //should have no effect -- pointer is already free
  
  myFree(q+1); //should fail -- q+1 does not point to head of block
  myFree(q);
  memoryMap();
  
  void *toobig = myMalloc(HEAP_SIZE + 1); // should fail -- too big
  if(toobig != NULL) {
    printf("Test failed: request to allocate too much space should return null.\n");
  }
  
  //Should not have issues allocating in spite of above failures
  char *s[4];
  int toalloc[4] = { 250000,480000,50000,100000};
  int i;
  
  for(i=0; i<4; i++) {
    s[i] = myMalloc(toalloc[i]);
  }
  
  memoryMap();
  myFree(s[3]);
  myFree(s[1]);
  memoryMap();
  myFree(s[0]);
  myFree(s[2]);
  memoryMap();
  
  // Para cronometro
  STOP_STOPWATCH(sw);

  printf("%f milisegundos\n", sw.mElapsedTime);

  //we have freed everything, so we should just have one block of size HEAP_SIZE
  
  /* Don't do this with DEBUG on
   
  
  int *bigarray[100000];
  int limit = 100000;
  for(i=0; i<limit;i++) {
    bigarray[i] = myMalloc(sizeof(int));
  }
  printf("Finished allocating 100000 ints\n");
  for(i=0; i<limit; i++) {
    myFree(bigarray[i]);
  }
  
  printf("Finished freeing 100000 ints\n");
  */
  
  return 0;
}