#include <unistd.h>
#include "memory.h"

#define HEADER_SIZE sizeof(Header)
#define NUNITS(bytes) bytes / HEADER_SIZE
#define MIN_ALLOC HEADER_SIZE * 2

struct header {
	unsigned int size; //tamanho do bloco de memória em bytes
	struct header *next; //proximo bloco de memória na free list
};
typedef struct header Header;

static Header* increase_heap(unsigned int);

static Header *free_list = NULL;
static Header base;

void* MyMalloc(size_t bytes)
{
	Header *block;
	Header *previous = &base;
	unsigned int nunits = NUNITS(bytes);
	if(!nunits)
		nunits = 1;
	if(!free_list){
		//primeira chamada
		Header *block = increase_heap(nunits);
		block->next = &base;
		base.size = 0;
		base.next = block;
		free_list = &base;
	}
	//procura um bloco de memória com tamanho suficiente
	for(block = base.next; block != &base; previous = block, block = block->next ){
		if(nunits <= block->size){
			if(nunits == block->size){ //o bloco de memória atual tem o tamanho exato \o/
				previous->next = block->next;
				block->next = NULL;
				return block + 1;
			}
			//o bloco é maior, então vamos dividi-lo
			Header *remain_block = block + 1 + nunits;
			remain_block->size = block->size - 1 - nunits;
			remain_block->next = block->next;
			previous->next = remain_block;
			block->size = nunits;
			return block + 1;
		}
	}
	//preciamos de mais memória...
	block = increase_heap(nunits);
	return block + 1;
}

void MyMallocFree(void* ptr)
{
	Header *fblock = ((Header*)ptr) - 1;
	Header *block;
	Header *previous = &base;
	for(block = base.next; block != &base; previous = block, block = block->next){
		//Vamos achar um bloco de memória adjacente
		if((block + 1 + block->size) == fblock){
			block->size += fblock->size;
			ptr = NULL;
			return;
		} else if((fblock + 1 + fblock->size) == block){
			fblock->size += block->size + 1;
			fblock->next = block->next;
			previous->next = fblock;
			ptr = NULL;
			return;
		}
	}
	previous->next = fblock;
	fblock->next = &base;
	ptr = NULL;
}

Header* increase_heap(unsigned int units)
{

	Header *block;
	if(units <= 1)
		block = (Header*) sbrk(MIN_ALLOC);
	else
		block = (Header*) sbrk( (1 + units) * HEADER_SIZE);
	
	block->size = units;
	block->next = NULL;
	return block;
}