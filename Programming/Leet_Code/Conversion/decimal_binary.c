#include "conversion.h"


int decimal_binary( const long int num)
{
    unsigned int num1 = num;
    int i = 0;
    int j = 0;
    unsigned int *ptr;
    ptr = (unsigned int*) malloc(sizeof(size_t));
    /*
    if (num > INT_MAX || num < INT_MIN)
    {
        return -EINVAL;
    }
    */
    if (ptr == NULL)
    {
        return -EINVAL;
    }
    while ( num1 > 0 )
    {
        *(ptr + i) = num1 % 2;
        num1 = num1 / 2;
        i++;
    }
    printf("Binary representation is:\n");
    for ( j = i-1; j >= 0; j-- )
    {
        printf(" %u ", *(ptr + j));
    }
    return 0;
}
int main(void)
{
    unsigned int num = 0;
    printf("Enter the num in decimal to convert it to the binary:");
    scanf("%u", &num);
    unsigned int *arr;
    decimal_binary(num);
}