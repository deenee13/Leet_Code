#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

struct node {
    char data;
    struct node *next;
};

union {
    uint16_t word;
    struct {
        uint8_t byte1;
        uint8_t byte2;
    };
}byte_swap;

void push(char byte_t);
void pop();
char top();
bool isempty();

struct node *front = NULL;

void push(char byte_t) {
    struct node *temp = (struct node*)malloc(sizeof(struct node));
    temp->data = byte_t;
    temp->next = front;

    front = temp;
}

void pop() {
    struct node *temp = front;

    front = front->next;
    free(temp);
}

char top() {

    if (front != NULL) {
        return front->data;
    } else {
        printf("stack is empty\n");
        exit(EXIT_FAILURE);
    }
}

bool isempty() {

    if (front == NULL) {
        return true;
    } else {
        return false;
    }
}

uint32_t swap_endianess_stack(uint32_t value) {
    char *ptr = NULL;
    ptr = (char*) &value;

    for (int i = 0; i < sizeof(uint32_t); i++) {
        push(*(ptr + i));
    }

    for (int i = 0; i < sizeof(uint32_t); i++) {
        *(ptr + i) = top();
        pop();
    }
    return value;
}


uint32_t swap_endianess_byte_reversal(uint32_t value) {
    char *ptr = NULL;
    ptr = (char*)&value;

    char count = sizeof(uint32_t);

    char temp;

    for (int i = 0; i < count/2; i++) {

        temp = *(ptr + i);
        *(ptr + i) = *(ptr + (count - 1 -i));
        *(ptr + (count - 1 -i)) = temp;
    }
    return value;
}

void check_endianess_union() {

    byte_swap.byte1 = 0x21;
    byte_swap.byte2 = 0x34;

    if (byte_swap.word == 0x3421) {
        printf("Architecutre is little endian\n");
    } else if (byte_swap.word == 0x2134) {
        printf("Architecture is big endian\n");
    } else {
        printf("Couldn't find the endianess\n");
    }
}

void check_endianess() {

    unsigned int value = 0;
    value = 0x12345678;

    char *ptr = (char*)&value;

    // if First byte is MSB then it is big endian
    // if First byte is LSB then Little endian
    if (*ptr == (value & 0xFF000000)) {
        printf("Big Endian architecture\n");
    } else {
        printf("Little Endian architecture\n");
    }
}


int main(void) {
    uint32_t value = 0x12345678;
    check_endianess_union();
    value = swap_endianess_byte_reversal(value);
    printf("Value is 0x%x\n", value);
}

