/* gcc -o set_clear_toggle set_clear_toggle.c*/
#include "set_clear_toggle.h"

int set_bit(unsigned int *num, uint8_t bit_value)
{
    /* How to find the number of bits in Arch? */
    if (bit_value > (8*sizeof(size_t) - 1))
    {
        return -EINVAL;
    }
    /* NULL Check on pointer */
    if (num == NULL)
    {
        // Verify it once
        return -EINVAL;
    }
    // To ste bit make use of the logical OR Operation
    *num |= (1 << bit_value);

    return 0;
}

int clear_bit(unsigned int *num, uint8_t bit_value)
{
    if (bit_value > (8*sizeof(size_t) - 1))
    {
        return -EINVAL;
    }
    // To clear bit make use of the logical AND with NOT Operation
    *num = *num & ~(1 << bit_value);

    return(0);
}

int toggle_bit(unsigned int* num, uint8_t bit_value)
{
    if (bit_value > (8*sizeof(size_t) - 1))
    {
        return -EINVAL;
    }
    // To Toggle bit make use of the logical EX-OR Operation
    *num = *num ^ (1 << bit_value);

    return(0);
}

int main(void)
{
    int flag = 0;
    unsigned int num = 0;
    num = 0x0C;
    unsigned int *ptr;
    ptr = &num;
    printf("––––––––––––––SET_BIT Function––––––––––––––");
    flag = set_bit(ptr,  0);
    printf("value is %02X and flag is %d\n", *ptr, flag);
    printf("\n");
    printf("––––––––––––––CLEAR_BIT Function––––––––––––––");
    flag = clear_bit(ptr, 0);
    printf("value is %02X and flag is %d\n", *ptr, flag);
    printf("\n");
    printf("–––––––––––––––––TOGGLE_BIT Function––––––––––––––");
    flag = toggle_bit(ptr, 2);
    printf("value is %02X and flag is %d\n", *ptr, flag);
    printf("\n");
}