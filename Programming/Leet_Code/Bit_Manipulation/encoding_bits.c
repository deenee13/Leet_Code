/*  gcc -o encoding_bits encoding_bits.c */
#include "bits_manipulation.h"

/**
 * This is one way to reverse the bits given the total number of bits 
 * and the number to reverse. In this Solution we left shift each bit to bring
 * it to the LSB position and then that specific bit is right shifted to the
 * appropriate reversed position. The position where this bit fits is obtained 
 * by subtracting the total bits from the i th position of the bit  
 * 
 **/

int reverse_bits(unsigned int *num) {
    unsigned int temp = 0;
    unsigned int reverse_num = 0;
    unsigned NUMBER_OF_BITS = 0;
    NUMBER_OF_BITS = 8*sizeof(*num);
    printf("Number before reversing is %X\n", *num);
    for (int i=0; i < NUMBER_OF_BITS; i++) {
        // Left shift the actual value by i times
        // To bring the each bit to LSB and then AND with 1 to
        // extract the specific bit
        temp = (*num >> i) & 0x01;
        // Then right shift the extracted bit to the equivalent MSB Position
        // Position to insert the extracted bit is obtained by
        // Subtracting the total bits with i
        reverse_num = reverse_num | temp << ((NUMBER_OF_BITS - 1) - i);
    }
    *num = reverse_num;
    printf("reversed number is %0X\n", reverse_num);
    return 0;
}

/**
 * In this approach we only reverse the bits where there is 1 present where in the above approach
 * we were shifting irrespective of whether the bit is 0 or 1.
 * 
 **/ 

int reverse_bits_differentway(unsigned int *num) {
    unsigned int temp = 0;
    unsigned int reverse_num = 0;
    unsigned NUMBER_OF_BITS = 0;
    // This will give the total number of the bits WRT the datatype
    NUMBER_OF_BITS = 8*sizeof(*num);
    printf("Number before reversing is %X\n", *num);
    for (int i=0; i < NUMBER_OF_BITS; i++) {
        temp = *num & (0x01 << i);
        // If we obtain 1 then only shift the bit and place it
        // at the appropriate reversed position
        if (temp) {
            reverse_num = reverse_num | 1 << ((NUMBER_OF_BITS - 1) - i);
        }
    }
    *num = reverse_num;
    printf("Number after reversing is %X\n", *num);
    return 0;
}

/** TODO:Think of generalising it what if we use random bit_value **/
int encoding_bits(unsigned int *num, unsigned int bit_value) {
    // Storing the lower 16 bits in temp variable
    unsigned int temp1 = *num & 0xFFFF;
    // Right shift the number with bit_Value
    *num = *num >> bit_value;
    //// reverse_bits(num);
    reverse_bits_differentway(num);
    *num = *num | temp1;
    return 0;
}

int main(void) {
    unsigned int bits_value;
    unsigned int *ptr;
    unsigned int num;
    printf("Enter the Most Significant bits to be reversed:");
    scanf("%d", &bits_value);
    printf("Enter the number in HEX:");
    scanf("%X", &num);
    printf("%hhu and %X\n", bits_value, num);
    ptr = &num;
    encoding_bits(ptr, bits_value);
    printf("shifted number is %0X\n", *ptr);
}
