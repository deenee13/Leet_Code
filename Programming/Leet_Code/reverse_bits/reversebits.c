#include <stdio.h>
#include <stdint.h>


uint32_t reverseBits(uint32_t n) 
{
    uint32_t mask = 0x1;
    uint32_t temp;
    uint32_t final_value = 0;

    for (int power = ((sizeof(unsigned int) * 8) - 1); power >= 0; power--)
    {   
        temp = (n & mask);
        temp = (temp << power);
        final_value = (temp | final_value);
        n = n >> 1;
    }

    
    return(final_value);
}


int main()
{
    uint32_t value = 0;
    printf("value to reverse is:");
    scanf("%d", &value);

    printf("Value passed is %u\n", value);
    value = reverseBits(value);

    printf("reversed value: %u ", value);
}