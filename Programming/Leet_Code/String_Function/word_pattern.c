#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/**
 * Still there is some bug instead of first alphabet
 * check whole word. or check the sum of the word 
 * which is one and the same
 * 
*/

struct node {
    int key;
    int value;
    struct node *next;
};

struct table {
    
    unsigned int size;
    struct node **next;
};

struct table *create_table(unsigned int size) {
    
    struct table *t = (struct table*)malloc(sizeof(struct table));
    
    t->size = size;
    
    t->next = (struct node **)malloc(size*sizeof(struct node*));
    
    for (int i = 0; i < size; i++) {
        
        t->next[i] = NULL;
    }
    return t;
}


int hash_code(struct table *t, int key) {
    
    if (key < 0) {
        return - (key % t->size);
    } else {
        
        return (key % t->size);
    }
}

bool insert(struct table *t, int key, int value) {
    
    int position = hash_code(t, key);
    
    struct node *list = t->next[position];
    
    struct node *temp = list;
    
    while (temp) {
        
        if (temp->key == key) {
            
            if (temp->value != value) {
                return false;
            }
            
            
        }
        temp = temp->next;
    }
    
    // Creating a new node to store the new entry
    struct node *newnode = (struct node*)malloc(sizeof(struct node));

    // If the key is not present then add the entry
    // at the front of the list and update the head
    newnode->key = key;
    newnode->value = value;

    // Updating the address of the previous entry by entering
    // it into the next field of the new node
    newnode->next = list;

    // Updating the array of heads
    t->next[position] = newnode;
    
    return true;
}



bool wordPattern(char * pattern, char * s) {

    struct table *t = NULL;
    bool result;
    t = create_table(5);

    int length = strlen(pattern);
    printf("%d\n", length);

    for (int i = 0; i < 4; i ++) {
        printf("value is %c and %c and %d\n", *(pattern + i ), *s, i);
        result = insert(t, *(pattern + i), *s);

        if ( result == false) {
            return false;
        }

        while (*s == '\0' || *s != ' ') {

            s++;
        }
        s++;

    }
    return true;

}

int main(void) {

    char pattern[] = "aaaa";
    char s[] = "dog dog dog fish";

    bool result = wordPattern(pattern, s);

    printf(" value is %d\n", result);
}




























