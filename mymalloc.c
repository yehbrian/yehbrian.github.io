#include <stdio.h>
#include <stdlib.h>
//#include "malloc.h"

struct block{
int valid; //checks if spot is valid 1 - true 0 - false
size_t size; //size of block
int free; // checks if the block is free or not
struct block *prev, *next;
};

void free(char* x, int y, char* z);

static char myblock[10000];

int main(){
	return 0;
}

static struct block* root;
void* Malloc(unsigned int size, int line, char* file){

if(size <= 0){
 errors(1, __LINE__, __FILE__);
return NULL;
}

static int initialized = 0;
struct block *p, *prev, *next;
if(!initialized){
root = (struct block*)myblock;
root->valid = 0; //not valid
root->prev = NULL;
root->next = NULL;
root->free = 1;
root->size = 10000-sizeof(struct block);
initialized = 1;
}

p = root;
do{
	if((p->size + sizeof(struct block)) < size || p->free ==0) //block is too small or not free so move on to next block
		p = p->next;
	else{
		next = (struct block*)((char*)p +sizeof(struct block) + size); //calculates the address of where next block will begin.
		next->free = 1; //next is free.
		next->valid = 0; //next is not valid to free by user.
		next->prev = p; //setting next's previous pointer to the block we will be allocating.
		next->size = (int)(p->size - size - (sizeof(struct block))); //calculating the size of next free block
		next->next = p->next;
		if(p->next != NULL){
			p->next->prev = next; //if the block allocated is not at the end of the list, then the previous pointer of the next block is the block we are currently allocating. (confusing)
		}
		p->next = next; 
		p->size = size;
		p->free = 0; //not free anymore
		p -> valid = 1 ; //This block is valid now
		printf("Allocated memory succesful. Memory address returned: %p size: %d \n" ((char*)p)+sizeof(struct block), p->size);
		return (char*)p+sizeof(struct block); //address we are returning is where the meta data ends.
	}
}while (p != NULL);

errors(1, __LINE__, __FILE__);
return NULL;
}

void merge( struct block* curr, struct block* prev,struct block* next){
        if(prev != NULL && prev->free == 1 ){ //Prev is not null and is free 
                prev->next = curr->next;
                prev->size = sizeof(struct block) + prev->size + curr->size;
                prev -> valid = 1;
                curr = prev;
                prev = NULL;
        }//connected previous block and current block and current now points to where previous was
        if(next != NULL && next->free == 1){
                curr->next = next->next;
                curr->size = sizeof(struct block) + curr->size + next->size;
                next->valid = 1;
                next->prev = NULL;
                next = NULL;
        }
}



void free(char* ptr, int line, char* file){
	if(ptr!= NULL){ //checking if user entered null pointer
		if(ptr >((char*)root) && ptr < ((char*)root + 10000)){ //checking if it is in the range of our 10000 byte block
			char* temp = ptr - (2*sizeof(struct block*)) - sizeof(int); //temp is pointing to free field of the block
			 if ( *(int*)(ptr - (2*sizeof(struct block*)) - (2*sizeof(int)) - sizeof(size_t))== 1){ //checks valid field of block
				if((*((int*)temp)) == 0){
					*temp = 1; //setting free field for this block to 1.
				//	*(int*)(ptr - (2*sizeof(struct block*)) - (2*sizeof(int)) - sizeof(size_t)) = 0; //setting valid field of the block to 0
					merge((struct block*)(ptr), ptr -> prev, ptr -> next);
				}
				else{
					errors(6, __LINE__, __FILE__);
				}
				
			}	
			else{
				//Trying to free something not allocated before because valid field is 0 
				//
				errors(5, __LINE__, __FILE__);
			}
	
		}
		else
		errors(3, __LINE__, __FILE__);
	}

	errors(4, __LINE__, __FILE__);

 
}

void errors(int x, int line, char* file){
switch(x){
case 1:
	printf("ERROR at line %d in file %s! Requested 0 or less bytes of memory.\n", line, file);
	break;
case 2:
	printf("ERROR at line %d in file %s!! Memory not able to be allocated due to either not enough memory or fragmented memory.\n", line, file);
	break;
case 3: 
	printf("ERROR at line %d in file %s! pointer is outside of the static block of memory that we use to allocate memory.\n", line, file);
	break;
case 4:
	printf("ERROR at line %d in file %s! trying to free a null pointer.\n", line, file);
	break;
case 5:
	printf("ERROR at line %d in file %s! trying to free memory not allocated by malloc.\n", line, file);
	break;
case 6:
	printf("ERROR at line %d in file %s! Trying to free memory that was already freed.\n", line, file);
	break;
}


}

