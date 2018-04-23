/*****************************************
**gcc -g -c testemalloc.c 
*gcc -g -o testemalloc testemalloc.c mymalloc.o 
*****************************************/
#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int main(int argc, char *argv[])
{
    debug_print("bloco header size is %d\n",sizeof(bloco));
    
    //test that request for 0 returns null
    char *fail = (char *)MyMalloc(0);
    if(fail != NULL) {
      printf("Test failed: request to allocate 0 bytes should return null.\n");
    }
    
    MyFree(fail); // should have no effect -- pointer is null
    
    
    //test some simple allocations/frees
    
    char *r = (char *)MyMalloc(10000*sizeof(char));
    memoryMap();
    
    double *q = (double *)MyMalloc(50000*sizeof(double));
    memoryMap();
    
    MyFree(r);
    memoryMap();
    
    MyFree(r); //should have no effect -- pointer is already free
    
    MyFree(q+1); //should fail -- q+1 does not point to head of bloco
    MyFree(q);
    memoryMap();
    
    void *toobig = MyMalloc(bigSize + 1); // should fail -- too big
    if(toobig != NULL) {
      printf("Test failed: request to allocate too much space should return null.\n");
    }
    
    //Should not have issues allocating in spite of above failures
    char *s[4];
    int toalloc[4] = { 250000,480000,50000,100000};
    int i;
    
    for(i=0; i<4; i++) {
      s[i] = MyMalloc(toalloc[i]);
    }
    
    memoryMap();
    MyFree(s[3]);
    MyFree(s[1]);
    memoryMap();
    MyFree(s[0]);
    MyFree(s[2]);
    memoryMap();

    return 0;

}