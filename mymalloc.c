// Jack Anderson
// jja54

#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mymalloc.h"

// Don't change or remove these constants.
#define MINIMUM_ALLOCATION  16
#define SIZE_MULTIPLE       8
#define PTR_ADD_BYTES(ptr, byte_offs) ((void*)(((char*)(ptr)) + (byte_offs)))

//My extra functions
typedef struct Block //16 byte struct!!
{
	unsigned int size; //4 bytes
	unsigned int used; //4 bytes
	struct Block* next; //4 bytes
	struct Block* prev; //4 bytes
} Block;

struct Block* head = NULL;
struct Block* tail = NULL;
void* heap_at_start = NULL;

void print_linked_list()
{
	Block* block = NULL;
	int block_num = 0;
	for(block = head; block != NULL; block = block->next)
	{
		printf("Block %d is of size %u", block_num, (unsigned int)block->size);
		block_num++;
		if (block->used == 1)
		{
			printf(" and is USED! %u", (unsigned int)block);
			if (block == tail)
			{
				printf(" I'm the TAIL!\n");
			}
			else if (block == head)
			{
				printf(" I'm the HEAD!\n");
			}
			else printf("\n");
		}
		else
		{
			printf(" and is FREE! %u", (unsigned int)block);
			if (block == tail)
			{
				printf(" I'm the TAIL!\n");
			}
			else if (block == head)
			{
				printf(" I'm the HEAD!\n");
			}
			else printf("\n");
		}
	}
	printf("\n");
}

Block* best_fit(unsigned int size)
{
	if (head == NULL) //linked list hasn't started yet, so immediately leave
	{
		return NULL;
	}
	
	Block* current_best = NULL;
	int current_best_gap;
	//loop through the blocks
	Block* block = NULL;
	for(block = head; block != NULL; block = block->next)
	{
		if (block->used == 0)
		{
			if (block->size >= size)
			{

				if (current_best == NULL) //first iteration
				{
					current_best = block;
					current_best_gap = (block->size - size);
				}
				if (block->size == size)
				{
					return block; //if it's perfect size, just return that!
				}
				else if ((block->size - size) < current_best_gap)
				{
					current_best = block;
					current_best_gap = (block->size - size);
				}
			}
		}
		if (block == tail)
		{
			break;
		}
	}
	if (current_best == NULL) return NULL;
	else return current_best;
}

Block* create_block(unsigned int size, Block* last_block)
{
	//HERE IS WHERE WE GO INTO BEST FIT CHECK
	Block* block = best_fit(size);
	if (block == NULL) //was no best fit, need to bump up sbrk
	{
		void* struct_address = sbrk(sizeof(Block));
		Block* block = struct_address;
		block->size = size;
		block->next = NULL;
		block->used = 1;
		if (last_block != NULL)
		{
			block->prev = last_block;
			last_block->next = block;
		}
		else
		{
			block->prev = NULL;
		}
		tail = block;
		return block;
	}
	
	else
	{
		//time to check for splitting:
		if (block->size > size && (block->size - size) >= 32) //can be split
		{
			unsigned int old_size = block->size;
			//block is our current FULL allocated block
			block->size = size;
			block->used = 1;
			
			Block* new_freed = block;

			new_freed = PTR_ADD_BYTES(new_freed, sizeof(Block));
			new_freed = PTR_ADD_BYTES(new_freed, size);
			
			new_freed->used = 0;
			new_freed->size = (old_size - size - sizeof(Block));
			new_freed->prev = block;
			new_freed->next = block->next;
			
			block->next = new_freed;
		}
		else if ((block->size - size) < 32) //avoiding create degenerate block
		{
			block->used = 1;
			//don't change the size of the block, leave it as is
		}
		
		if (last_block == block) //reusing the tail
		{
			tail = block;
			block->next = NULL;
			//prev remains the same
			block->used = 1;
		}
		return block;
	}

}

unsigned int round_up_size(unsigned int data_size)
{
	if(data_size == 0)
		return 0;
	else if(data_size < MINIMUM_ALLOCATION)
		return MINIMUM_ALLOCATION;
	else
		return (data_size + (SIZE_MULTIPLE - 1)) & ~(SIZE_MULTIPLE - 1);
}

void* my_malloc(unsigned int size)
{
	if(size == 0)
		return NULL;

	size = round_up_size(size);

	// ------- Don't remove anything above this line. -------
	if (heap_at_start == NULL) //haven't started yet
	{
		heap_at_start = sbrk(0);
	}
	
	if (head == NULL) //first hasn't been created yet 
	{
		head = create_block(size, tail); //allocate the block node with sbrk
		tail = head;
		void* p = sbrk(size); //p points to a block of "size" bytes
		return p; //return the address of malloc'd block
	}
	
	//if not the first creation
	Block* new_block = create_block(size, tail); //allocate the block node
	if (new_block == tail)
	{
		void* p = sbrk(size); //p points to a block of "size" bytes
		return p; //return the address of malloc'd block
	}
	else
	{	
		unsigned int p; //bump up to give space where mem goes
		p = ((unsigned int)new_block + 16);
		return (void*)p;
	}
	
}

int check_adjacent_is_free(Block* block)
{
	if (block != NULL)
	{
		if (block->used == 0)
		{
			return 1;
		}
	}
	return 0;
}

Block* coalesce_next(Block* block, Block* next)
{
	unsigned int block_size = block->size;
	unsigned int next_size = next->size;
	unsigned int new_size = block_size + next_size + sizeof(Block);
	
	block->size = new_size; //change the size to the new
	block->next = next->next; //give the block the next of the next
	//prev remains the same
	if (next->next != NULL)
	{
		next->next->prev = block; //set new next's prev to this!
	}
	
	if (next == tail)
	{
		tail = block; //if next block was the tail, make the original block new tail
	}
	return block;
}

Block* coalesce_prev(Block* block, Block* prev)
{
	unsigned int block_size = block->size;
	unsigned int prev_size = prev->size;
	unsigned int new_size = block_size + prev_size + sizeof(Block);
	
	prev->size = new_size; //change the size to the new
	prev->next = block->next;
	
	if (block->next != NULL)
	{
		block->next->prev = prev; 
	}
	
	if (block == tail)
	{
		tail = prev; //if block was tail, prev becomes tail
		prev->next = NULL;
		if (prev->prev != NULL)
		{
			prev->prev->next = NULL; //new tail shouldn't have a next
		}
	}
	return prev;
}

Block* coalesce_both(Block* block, Block* next, Block* prev)
{
	unsigned int block_size = block->size;
	unsigned int prev_size = prev->size;
	unsigned int next_size = next->size;
	unsigned int new_size = block_size + sizeof(Block) + prev_size + next_size + sizeof(Block);
	
	prev->size = new_size;
	prev->next = next->next;
	if (next->next != NULL)
	{
		next->next->prev = prev; //set new next's prev to this!
	}
	
	if (next == tail)
	{
		tail = prev;
	}
	return prev;
}

void my_free(void* ptr)
{
	if(ptr == NULL) 
	{
		return;
	}
	
	
	Block* block = ptr - 16; //access the struct before the malloc'd block
	block->used = 0; //make it free
	
	//check if able to coalesce:
	Block* next = block->next;
	Block* prev = block->prev;
	int next_is_free = check_adjacent_is_free(next);
	int prev_is_free = check_adjacent_is_free(prev);
	if (next_is_free && prev_is_free)
	{
		block = coalesce_both(block, next, prev);
	}
	else if (next_is_free && !prev_is_free)
	{
		block = coalesce_next(block, next);
	}
	else if (prev_is_free && !next_is_free)
	{
		block = coalesce_prev(block, prev);
	}
	
	//now we have coalesced the blocks if possible, check if it was the tail
	
	if ((unsigned int)block == (unsigned int)tail) //deallocing last block, just move brk
	{	
		if (tail != head)
		{
			tail = tail->prev;
			block->prev->next = NULL;
		}
		
		unsigned int neg_size = (0 - (unsigned int)block->size - sizeof(Block));
		ptr = sbrk(neg_size); //bump back the break

		void* heap_at_bump_back = sbrk(0);
		if (heap_at_start == heap_at_bump_back) //our list is now empty
		{
			head = NULL;
			tail = NULL;
		}
	}
	
	else if (tail != NULL && (unsigned int)tail->used == 0)
	{
		unsigned int neg_size = (0 - (unsigned int)tail->size - sizeof(Block));
		ptr = sbrk(neg_size); //bump back the break
	}
	return;
}