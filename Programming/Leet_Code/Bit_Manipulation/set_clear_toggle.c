/* gcc -o set_clear_toggle set_clear_toggle.c*/
#include "bits_manipulation.h"

int set_bit(unsigned int *num, const uint8_t bit_value) {
    /* How to find the number of bits in Arch? */
    // Done below
    if (bit_value > (8*sizeof(size_t) - 1)) {
        return -EINVAL;
    }
    // NULL Check on pointer
    if (num == NULL) {
        return -EINVAL;
    }
    // To set bit make use of the logical OR Operation
    *num |= (1 << bit_value);

    return 0;
}

int clear_bit(unsigned int *num, const uint8_t bit_value) {
    if (bit_value > (8*sizeof(size_t) - 1)) {
        return -EINVAL;
    }
    // To clear bit make use of the logical AND with NOT Operation
    *num = *num & ~(1 << bit_value);

    return(0);
}

int toggle_bit(unsigned int* num, const uint8_t bit_value) {
    if (bit_value > (8*sizeof(size_t) - 1)) {
        return -EINVAL;
    }
    // To Toggle bit make use of the logical EX-OR Operation
    *num = *num ^ (1 << bit_value);

    return(0);
}

#define MASK_MSB 0x80000000U

/* 
 *  This is the one way of rotating the bits.
 *  But this is expensive way of doing it.
 */
int rotate_bits_unoptimised(unsigned int* num, uint8_t value) {

    /* This should be wide enough to capture MSB. */
    uint64_t temp = 0;
    if (value > (8*sizeof(int) - 1)) {
        return -EINVAL;
    }

    while (value != 0) {

        temp = *num & MASK_MSB;
        temp = temp >> (8*sizeof(int) - 1);

        *num = *num << 1;
        *num |= temp;
        value--;
    }
    return 0;
}


int rotate_bits(unsigned int* num, uint8_t value) {

    if (value > (8*sizeof(int) - 1)) {
        printf("%d\n", value);
        return -EINVAL;
    }

    *num =  ((*num >> (sizeof(int)*8 - value)) | (*num << value));
    return 0;
}

int main(void) {
    printf("No. of bits in the architecture are %lu\n", 8*sizeof(size_t)-1);
    int flag = 0;
    unsigned int num = 0;
    num = 0x0C;
    unsigned int *ptr;
    ptr = &num;
    printf("––––––––––––––SET_BIT Function––––––––––––––\n");
    flag = set_bit(ptr,  0);
    printf("value is %02X and flag is %d\n", *ptr, flag);
    printf("\n");
    printf("––––––––––––––CLEAR_BIT Function––––––––––––––\n");
    flag = clear_bit(ptr, 0);
    printf("value is %02X and flag is %d\n", *ptr, flag);
    printf("\n");
    printf("––––––––––––––TOGGLE_BIT Function––––––––––––––\n");
    flag = toggle_bit(ptr, 2);
    printf("value is %02X and flag is %d\n", *ptr, flag);
    printf("\n");
    printf("––––––––––––––ROTATE_BITS Function––––––––––––––\n");
    *ptr = 0X123456;
    flag = rotate_bits(ptr, 8);
    printf("value is %02X and flag is %d\n", *ptr, flag);
    printf("\n");
}
