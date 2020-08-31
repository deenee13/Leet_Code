/* gcc -o memory_deenee memory_deenee.c */
#include "memory_deenee.h"



int mem_copy_deep(void *dest, const void *source, size_t numbyte)
{
    int i;

    // Typecasting the NULL pointer
    char *cdest = (char *)(dest);
    const char *csource = (const char *)(source);
    // if (!source || !dest) {
    // if (source == NULL || dest == NULL) {
    // Checking the for NULL Pointer
    if (!(source && dest))
    {
        return -EINVAL;
    }

    if (numbyte == 0)
    {
        return 0;
    }
    printf("value of the numbyte is %zu\n", numbyte);

    /* Implement for faster copy using arch */
    for (i = 0; i < numbyte; i++)
    {
        *(cdest + i) = *(csource + i);
    }

    return 0;
}

int main(void)
{
    int flag = 0;
    unsigned int num = 0;
    num = 0x0C;
    unsigned int *ptr;
    ptr = &num;
    int n = 0;
    int arr[] = {10, 20, 30, 40, 50};
    int *idest = (int *)malloc( n );
    flag = mem_copy_deep( idest, arr, sizeof(arr));
    printf("Value of the flag is %d\n", flag);
    printf("copied array is:\n");
    for (int i=0; i < sizeof(arr); i++)
    {
        printf(" %d\n", *(idest + i));
    }
}