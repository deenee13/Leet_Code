/* https://danluu.com/malloc-tutorial/ */

#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>


/*
 * Structure used to track the block of memory
 */
struct block_meta {
    int free;
    int magic;  // For debugging purpose
    size_t size;
    struct block_meta *next;
};

#define META_SIZE sizeof(struct block_meta)

/*
 * Head to the linked list
 */
void *global_base = NULL;

/*
 * For malloc if we want to reuse the space which got free
 * during the program then we can again reuse it before programs
 * ends using this function
 */
struct block_meta *find_free_block(struct block_meta **last, size_t size) {

    struct block_meta *current = global_base;

    while (current && !(current->free && current->size >= size)) {

        *last = current;
        current = current->next;
    }
    return current;
}

/*
 * If we don't find a free block from the linked list we will need 
 * to request free space from OS using sbrk() and then add that to 
 * our linked list. 
 * What sbrk() does is that it will allow us to manipulate the heap 
 * section of the memory. It actually increments the heap by the memory 
 * we want say 4 bytes and then return the pointer to the previous location 
 * ie before increment.
*/

struct block_meta *request_space(struct block_meta* last, size_t size) {

    struct block_meta *block;
    /*
     *  This will point to the current address of the heap
     */
    block = sbrk(0);

    /**
     * requesting the block of memory from os
    */
    void* request = sbrk(size + META_SIZE);

    /**
     * sbrk failed to allocate memory
    */
    if (request == (void*)-1) {
        return NULL;
    }

    /**
     * asserting to check that block and request are same address.
     * They should be same to check whether sbrk allocated the memory.
    */
   assert((void*)block == request);

    /**
     * Putting it into the linked list. Adjusting the links
    */
    if (last) {
        last->next = block;
    }

    block->size = size;
    block->next = NULL;
    block->free = 0;
    block->magic = 0x12345678;

    return block;
}

struct block_meta *get_block_ptr(void *ptr) {
  return (struct block_meta*)ptr - 1;
}

/**
 * If our global base pointer is NULL, we need to request space and set the 
 * base pointer to our new block. If it's not NULL, we check to see if we can 
 * re-use any existing space. If we can, then we do; if we can't, then we request 
 * space and use the new space.
*/
void *malloc(size_t size) {

    struct block_meta *block;

    if (size <= 0) {
        return NULL;
    }

    /**
     * Calling malloc for first time
    */
    if (!global_base) {

        block = request_space(NULL, size);

        if (!block) {
            return NULL;
        }

        /**
         * making sure that global head has the address of the last node
        */
        global_base = block;

    } else {

        /**
         * passing the address of the last node via updated global base
        */
        struct block_meta *last = global_base;

        block = find_free_block(&last, size);

        if (!block) {  // Failed to find free block.

            block = request_space(last, size);

            if (!block) {
                return NULL;
            }

        } else {      // Found free block
            // <TODO:> consider splitting block here.
            block->free = 0;
            block->magic = 0x69696969;
        }
    }

    return(block+1);
}


void free(void *ptr) {
  if (!ptr) {
    return;
  }

  // <TODO:> consider merging blocks once splitting blocks is implemented.
  struct block_meta* block_ptr = get_block_ptr(ptr);
  assert(block_ptr->free == 0);
  assert(block_ptr->magic == 0x69696969 || block_ptr->magic == 0x12345678);
  block_ptr->free = 1;
  block_ptr->magic = 0x55555555;
}

void *calloc(size_t nelem, size_t elsize) {
  size_t size = nelem * elsize; // TODO: check for overflow.
  void *ptr = malloc(size);
  memset(ptr, 0, size);
  return ptr;
}

int main(void) {

    int value = 8;
    int* ptr = (int*)malloc(sizeof(int));

    int* ptr1 = (int*)malloc(sizeof(int));

    if (ptr == NULL && ptr1 == NULL) {

        printf("NULL\n");
    }
    *ptr = 69;
    printf("value is %d\n", *ptr);
    free(ptr);
}


































