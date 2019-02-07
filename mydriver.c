// Jack Anderson
// jja54

#include <stdio.h>
#include <unistd.h>

#include "mymalloc.h"

void* heap_at_start; //globals
void* heap_pre_malloc;
int num_of_mallocs = 0;

void print_block_info(void* block)
{
	void* heap_at_end = sbrk(0);
	unsigned int heap_inc_diff = (unsigned int)(heap_at_end - heap_pre_malloc);
	if(heap_inc_diff)
	{
		printf("Hmm, the heap got bigger by %u (0x%X) bytes...\n", heap_inc_diff, heap_inc_diff);
		printf("our block is at address %u...\n", (unsigned int)block);
		printf("Sooo, the allocated block size was %u\n\n", (unsigned int)(heap_at_end - block));
	}
	
	else
	{
		printf("There was no change in heap size!\n");
		printf("our block is at address %u...\n\n", (unsigned int)block);
	}
	return;
}

void print_end_info()
{
	void* heap_at_end = sbrk(0);
	unsigned int heap_size_diff = (unsigned int)(heap_at_end - heap_at_start);
	printf("Overall the heap got bigger by %u (0x%X) bytes,\n", heap_size_diff, heap_size_diff);
	printf("And there were a total of %d allocations!\n", num_of_mallocs);
	printf("The heap started at %u...\n", (unsigned int)heap_at_start);
	printf("and it ended at:    %u!\n", (unsigned int)heap_at_end);
	return;
}

int main()
{
	// You can use sbrk(0) to get the current position of the break.
	// This is nice for testing cause you can see if the heap is the same size
	// before and after your tests, like here.

	//OUTDATED TESTS
	heap_at_start = sbrk(0);
	heap_pre_malloc = sbrk(0);
	void* block = my_malloc(100);
	num_of_mallocs++;
	print_block_info(block);
	
	
	heap_pre_malloc = sbrk(0);
	void* block2 = my_malloc(10);
	num_of_mallocs++;
	print_block_info(block2);
	
	
	heap_pre_malloc = sbrk(0);
	void* block3 = my_malloc(80);
	num_of_mallocs++;
	print_block_info(block3);

	
	heap_pre_malloc = sbrk(0);
	void* block4 = my_malloc(64);
	num_of_mallocs++;
	print_block_info(block4);
	
	
	heap_pre_malloc = sbrk(0);
	void* block5 = my_malloc(64);
	num_of_mallocs++;
	print_block_info(block5);
	
	my_free(block3);
	my_free(block4);
	
	heap_pre_malloc = sbrk(0);
	void* block6 = my_malloc(56);
	num_of_mallocs++;
	print_block_info(block6);
	
	heap_pre_malloc = sbrk(0);
	void* block7 = my_malloc(56);
	num_of_mallocs++;
	print_block_info(block7);
	
	heap_pre_malloc = sbrk(0);
	void* block8 = my_malloc(16);
	num_of_mallocs++;
	print_block_info(block8);
	
	print_end_info();

	// ADD MORE STUFF HERE.

	return 0;
}

