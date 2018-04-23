#define HEAP_SIZE 1000000
#define SIGNATURE 0xca11ab1e
#define DEBUG 0 // caso nao seja necessario realizar o debug mudar para 0
/****************************************
* debug print: print responsavel por dar o
* retorno de commo esta ocorrendo a alocacao
****************************************/
#define debug_print(args ...) if (DEBUG) fprintf(stderr, args)
#include <stdbool.h>

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
typedef struct block {
  struct block *prox;
  struct block *ant;
  unsigned int size;
  bool free;
  long signum;
} Block;
/**********************************
int threadLocked;

void lockThread(){
	while(threadLocked == 1)
		usleep(1);
	threadLocked = 1;
}

void unlockThread(){
	threadLocked = 0;
}
**********************************/
void *myMalloc(unsigned int size);


void myFree(void *ptr);

void memoryMap();

void myMallocInit();
