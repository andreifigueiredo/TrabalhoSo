/*****************************************
*gcc -g -c mymalloc.c 
*****************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define SIGNATURE 0xca11ab1e
#define DEBUG 1 // caso nao seja necessario realizar o debug mudar para 0
/****************************************
* debug print: print resosavel por dar o
* retorno de commo esta ocorrendo a alocacao
****************************************/
#define debug_print(args ...) if (DEBUG) fprintf(stderr, args)
#define bigSize 1000000


/*****************************************
 
 * o bloco e uma struct simples que contem
 * ponteiros para o bloco anterior e o proximo
 * da heap, um campo indicando o tamanho alocado
 * e uma variavel boolean para indicar qnd esse
 * bloco esta livre ou alocado. Alem disso existe
 * uma variavel que indica quando o bloco foi
 * setado pelo MyMalloc para impedir blocos
 * livres inapropriados.

 *****************************************/

typedef struct bloco {
  struct bloco *prox;
  struct bloco *ant;
  unsigned int size;
  bool free;
  long signum;
} bloco;

static bloco *beginheap = NULL;

int MyMallocInit() {
	beginheap = (bloco *)sbrk(bigSize+sizeof(bloco));
  	beginheap->prox = NULL;
  	beginheap->ant = NULL;
  	beginheap->size = bigSize;
  	beginheap->free = true;
  	beginheap->signum = SIGNATURE;

  return 1;
}

void memoryMap() {
  bloco *p;
  for(p = beginheap; p != NULL; p=p->prox) {
    char* lead = p->free == true ? "Free bloco" : "Used bloco";
    printf("%s: addr=%p size=%d ant=%p prox=%p\n", lead, p, p->size, p->ant, p->prox);
  }
  printf("\n");
}

void *MyMalloc(unsigned int size)
{
  if (size <= 0) return NULL; // size deve ser maior que 0

  if(!beginheap) MyMallocInit(size); // MyMallocInit inicia a heap se ela nao existir
  
  size = size + sizeof(bloco);
  
  debug_print("Trying to allocate bloco of size %d\n", size);
  
  
  bloco *p;
  
  for(p = beginheap; p != NULL; p = p->prox)
    if(p->size >= size && p->free == true) { // percorre a heap ate encontrar um bloco e tamanho suficiente
      break;
    }
    
    if(p == NULL || p->size < size) { // se o bloco for menor que o tamanho que queremos usar não o utilizaremos...
      debug_print("Failed to find enough free space!\n\n");
      return NULL;
    }
    
    if(p->size == size) { // se o bloco for do tamanho que queremos, so temos que marcar ele como usado
      debug_print("exact match: allocating bloco of size %d\n", size);
      p->free = false;
      p->signum = SIGNATURE;
    } else { // precisamos dividir esse bloco em um usado e um livre
      bloco *newfreebloco = (bloco *)((void*)p + size);
      debug_print("allocating bloco of size %d from address %p to %p\n", size, p, newfreebloco);
      
      // esse sera o novo bloco livre
      newfreebloco->size = p->size - size;
      newfreebloco->prox = p->prox;
      newfreebloco->ant = p;
      newfreebloco->signum = SIGNATURE;
      if(newfreebloco->prox) {
	newfreebloco->prox->ant = newfreebloco;
      }
      newfreebloco->free = true;
      
      //esse sera o bloco usado
      p->size = size;
      p->free = false;
      p->prox = newfreebloco;
      p->signum = SIGNATURE;
    }
    
    
    debug_print("Returning pointer at %p\n", (void*)(p+1)); 
    return (void*)(p + 1);
    
}

void myFree(void *ptr) {
  if(ptr == NULL) { // não tem nada para dar free
    debug_print("Invalid pointer passed to free -- cannot free null pointer\n\n");
    return;
  }
  
  bloco *beginfreebloco = (bloco*)(ptr - sizeof(bloco)); 
  if(beginfreebloco->free == true || beginfreebloco->size == 0) { // nao faz nada
    debug_print("Invalid pointer passed to free -- bloco already freed.\n\n");
    return;
  }
  if(beginfreebloco->signum != SIGNATURE) { // aponta para memoria invalida -- nao faz nada
    debug_print("Invalid pointer passed to free -- signature does not match.\n\n");
    return;
  }
  
  debug_print("Now freeing bloco: addr=%p size=%d ant=%p prox=%p\n\n", beginfreebloco, beginfreebloco->size, beginfreebloco->ant, beginfreebloco->prox);
  
  beginfreebloco->free = true;
  
  //se tem um bloco livre em seguida, deve-se unir esses dois blocos
  
  if(beginfreebloco->prox != NULL && beginfreebloco->prox->free == true) {
    bloco *proxbloco = beginfreebloco->prox;
    if(proxbloco->prox != NULL) {
      bloco *twoahead = proxbloco->prox;
      twoahead->ant = beginfreebloco;
    }
    beginfreebloco->size = beginfreebloco->size + proxbloco->size;
    beginfreebloco->prox = proxbloco->prox;
    debug_print("Merged following free bloco\n");
    debug_print("New bloco is: addr=%p size=%d ant=%p prox=%p\n\n", beginfreebloco, beginfreebloco->size, beginfreebloco->ant, beginfreebloco->prox);
  }
  
  //se tiver um bloco livre antes, deve-se unir esses dois blocos
  if(beginfreebloco->ant != NULL && beginfreebloco->ant->free == true) {
    bloco *antbloco = beginfreebloco->ant;
    if(beginfreebloco->prox != NULL) {
      bloco *proxbloco = beginfreebloco->prox;
      antbloco->prox = proxbloco;
      proxbloco->ant = antbloco;
    } else {
      antbloco->prox = NULL;
    }
    antbloco->size = antbloco->size + beginfreebloco->size;
    antbloco->free = true;
    debug_print("Merged leading free bloco\n");
    debug_print("New bloco is: addr=%p size=%d ant=%p prox=%p\n\n", antbloco, antbloco->size, antbloco->ant, antbloco->prox);
  }
  
}