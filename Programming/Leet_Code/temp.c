#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

unsigned int count = 0;
typedef struct {
    int value;
    struct RandomizedSet* next;
} RandomizedSet;

/** Initialize your data structure here. */

RandomizedSet* randomizedSetCreate() {
    
    RandomizedSet* head = (RandomizedSet*)malloc(sizeof(RandomizedSet));
    return head;
}

void print(RandomizedSet *head) {
    RandomizedSet *temp = head;
    printf("List:");
    while (temp != NULL) {
        printf(" %d ", temp->value);
        temp = temp->next;
    }
    printf("\n");
}

/** Inserts a value to the set. Returns true if the set did not already contain the specified element. */
bool randomizedSetInsert(RandomizedSet* obj, int val) {
        
    
    RandomizedSet* temp = obj;
    while (temp != NULL) {

        if (temp->value == val) {
            return false;
        }
        temp = temp->next;
    }
    
    RandomizedSet* temp1 = (RandomizedSet*)malloc(sizeof(RandomizedSet));
    temp1->value = val;
    temp1->next = NULL;
    temp->next = temp1;
    count++;
    print(obj);
    return true;    
}

/** Removes a value from the set. Returns true if the set contained the specified element. */
bool randomizedSetRemove(RandomizedSet* obj, int val) {
    
    unsigned int count = 1;
    unsigned int position = 0;
    
    RandomizedSet* temp = obj;
    while (temp != NULL) {
        
        if (temp->value == val) {
            position = count;
        }
        temp = temp->next;
    }
    
    temp = obj;
    if (position != 0) {
        
        for (int i = 0; i < position - 2; i++) {
            temp = temp->next;
        }
        RandomizedSet* current = temp->next;
        temp->next = current->next;
        count--;
        free(current);
        return true;
    } else {
        printf("in else\n");
        return false;
    }
}

/** Get a random element from the set. */
int randomizedSetGetRandom(RandomizedSet* obj) {
    
    print(obj);
    int position = (rand() % count);
    printf("Value of position is %d and count %d\n", position, count);
    for (int i = 0; i < position - 1; i++) {
        obj = obj->next;
    }
    return obj->value;
}

void randomizedSetFree(RandomizedSet* obj) {
    
}

int main(void) {
  RandomizedSet* obj = randomizedSetCreate();
  bool param_1 = randomizedSetInsert(obj, 1);
  
  bool param_2 = randomizedSetRemove(obj, 2);

  bool param_4 = randomizedSetInsert(obj, 3);
 
  int param_3 = randomizedSetGetRandom(obj);
 
  randomizedSetFree(obj);
}

/*

//
// Data types
//
typedef void * PVOID;
typedef unsigned long UINT;
typedef listEntry * PLISTENTRY;

typedef struct _listEntry{
    PLISTENTRY Flink;
    PLISTENTRY Blink;
}listEntry, *listEntry;

//
// Macros
//
#define OFFSET(TYPE, MEMBER) \
            (UINT) &(((TYPE*)0)->MEMBER)

//
// Function Declarations
//
PVOID allocateMemory(
    UINT bufferSize
);

void initializeListHead(
    PLISTENTRY listHead
);
void insertListTail(
    PLISTENTRY listHead,
    PLISTENTRY entry
);

UINT generateUnalignedAddress(
    void
);
UINT generateAlignedAddress32(
    void
);
UINT generateAlignedAddress64(
    void
);

UINT alignAddress128B(
    UINT addr
);

//
// Address allocator
//
typedef UINT (*addressAlloc) (void);

addressAlloc addressAllocator[3] = {
    generateUnalignedAddress,
    generateAlignedAddress32,
    generateAlignedAddress64
};

//
// Data Structure
//
typedef linkCounter * PLINKCOUNTER;

typedef struct _linkCounter{
    UINT xyz;
    UINT refCount;
    PLISTENTRY entry;
}linkCounter, *linkCounter;


//
// Globals
//
UINT gCounter = 0;

int main()
{
    PLISTENTRY head = NULL;
    
    initializeListHead(head);
    
    for (UINT cnt=0; cnt<5; cnt++)
    {
        PLINKCOUNTER counter;
        
        counter = (PLINKCOUNTER)allocateMemory(sizeof(linkCounter));

        gCounter++;
        counter->refCount = gCounter;

        /*
        1. Read addr from generateAlignedAddress32 using addressAllocator
        function pointer.
        */
        counter->xyz = /*???*/addressAllocator[1]();

        insertListTail(head, counter->entry);
    }
    PLISTENTRY temp = head;
  	head = temp->Blink;
    
    head = head->Blink;
	
		(PLINKCOUNTER)(head - OFFSET(PLINKCOUNTER,entry))->refcount
    
    /*
                 []   []   []   []   []
                 []   []   []   [@]  []
        <..[H]<->[]<->[]<->[]<->[]<->[]<..
        
        Above chain links struct linkCounter in doubly linked list.
        2. Read back the entry (refCount) of the entry represented by @
        3. If addr of struct linkCounter contains unaligned 64bit address,
        implement the function alignAddress128B which aligns address to 128B.
        4. Rewrite the above for concurrent access.
    */
    
    0XDEADBEEFDEADBEEF
    
    _________
    |
    |
   |
 ->|
   |
   |
 ->-----------
    
       [xyz]<-
       [ref]
    -> [entry]
    return 0;
}





*/