/*****************************************
*gcc -g -c mymalloc.c 
*****************************************/
#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

static Block *beginheap = NULL;
//int threadLocked = 0;

void myMallocInit() {
    beginheap = (Block *)sbrk(HEAP_SIZE+sizeof(Block));
    beginheap->prox = NULL;
    beginheap->ant = NULL;
    beginheap->size = HEAP_SIZE;
    beginheap->free = true;
    beginheap->signum = SIGNATURE;
}

void memoryMap() {
    Block *p;
    for(p = beginheap; p != NULL; p=p->prox) {
      char* lead = p->free == true ? "Free block" : "Used block";
      printf("%s: addr=%p size=%d ant=%p prox=%p\n", lead, p, p->size, p->ant, p->prox);
    }
    printf("\n");
}

void *myMalloc(unsigned int size){
    if (size <= 0) 
      return NULL; // size deve ser maior que 0

//    lockThread();
    
    if(!beginheap) 
      myMallocInit(); // myMallocInit a heap se nao existe
    
    size = size + sizeof(Block);
    
    debug_print("Tentando alocar block de size %d\n", size);
    
    
    Block *p;
    
    for(p = beginheap; p != NULL; p = p->prox)
      if(p->size >= size && p->free == true) { // percorre a heap ate encontrar block livre grande o suficiente
        break;
      }
      
      if(p == NULL || p->size < size) { // fim da heap, entao nao tem block suficiente
        debug_print("Falha em encontrar espaco livre suficiente!\n\n");
        return NULL;
      }
      
      if(p->size == size) { // tudo que deve-se fazer nesse caso e mudar o estado do block para usado
        debug_print("Tamanho exato: alocando block de size %d\n", size);
        p->free = false;
        p->signum = SIGNATURE;
      } 
      else { // preciso dividir o block em dois: um usado e um livre
        Block *newfreeblock = (Block *)((void*)p + size);
        debug_print("alocando block de size %d do endereco %p para %p\n", size, p, newfreeblock);
        
        // esse sera o novo block livre
        newfreeblock->size = p->size - size;
        newfreeblock->prox = p->prox;
        newfreeblock->ant = p;
        newfreeblock->signum = SIGNATURE;
        if(newfreeblock->prox) {
  	     newfreeblock->prox->ant = newfreeblock;
        }
        newfreeblock->free = true;
        
        //esse e o espaaco usado -- precedera o block livre
        p->size = size;
        p->free = false;
        p->prox = newfreeblock;
        p->signum = SIGNATURE;
      }
    
 //     unlockThread();
      
      debug_print("Returning pointer at %p\n", (void*)(p+1)); // return poteiro para espaco usavel
      return (void*)(p + 1);
      
}

void myFree(void *ptr) {
    if(ptr == NULL) { // nada e feito
      debug_print("Invalid pointer passed to free -- cannot free null pointer\n\n");
      return;
    }

//    lockThread();
    
    Block *beginfreeblock = (Block*)(ptr - sizeof(Block)); // deve apontar para o header
    if(beginfreeblock->free == true || beginfreeblock->size == 0) { // nada e feito
      debug_print("Invalid pointer passed to free -- block already freed.\n\n");
      return;
    }
    if(beginfreeblock->signum != SIGNATURE) { // aponta para memoria invalida -- nada e feito
      debug_print("Invalid pointer passed to free -- signature does not match.\n\n");
      return;
    }
    
    debug_print("Now freeing block: addr=%p size=%d ant=%p prox=%p\n\n", beginfreeblock, beginfreeblock->size, beginfreeblock->ant, beginfreeblock->prox);
    
    beginfreeblock->free = true;
    
    //se exite block livre antes ou apos, deve se unir os block
    
    if(beginfreeblock->prox != NULL && beginfreeblock->prox->free == true) {
      Block *proxblock = beginfreeblock->prox;
      if(proxblock->prox != NULL) {
        Block *twoahead = proxblock->prox;
        twoahead->ant = beginfreeblock;
      }
      beginfreeblock->size = beginfreeblock->size + proxblock->size;
      beginfreeblock->prox = proxblock->prox;
      debug_print("Merged following free block\n");
      debug_print("New block is: addr=%p size=%d ant=%p prox=%p\n\n", beginfreeblock, beginfreeblock->size, beginfreeblock->ant, beginfreeblock->prox);
    }
    
    if(beginfreeblock->ant != NULL && beginfreeblock->ant->free == true) {
      Block *antblock = beginfreeblock->ant;
      if(beginfreeblock->prox != NULL) {
        Block *proxblock = beginfreeblock->prox;
        antblock->prox = proxblock;
        proxblock->ant = antblock;
      } 
      else {
        antblock->prox = NULL;
      }
      antblock->size = antblock->size + beginfreeblock->size;
      antblock->free = true;
      debug_print("Merged leading free block\n");
      debug_print("New block is: addr=%p size=%d ant=%p prox=%p\n\n", antblock, antblock->size, antblock->ant, antblock->prox);
    }
//    unlockThread();
}