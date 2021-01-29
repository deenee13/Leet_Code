/* gcc -o memory_deenee memory_deenee.c */
#include "memory_deenee.h"



// TODO(deenee): Why Getting Junk values in mem move
int mem_copy_deep(void *dest, const void *source, size_t numbyte) {
    int i;

    // Typecasting the NULL pointer
    char *cdest = (char *)(dest);
    const char *csource = (const char *)(source);
    // Checking for NULL Pointer
    if (source == NULL || dest == NULL) {
        return -EINVAL;
    }
    // If  there is no data to copy then return 0
    if (numbyte == 0) {
        return 0;
    }
    // Check for the overlapping condition
    if ( csource < cdest && cdest < (csource + numbyte) ) {
        printf("Source and the destination location are Overlapping\n");
        return -EINVAL;
    }
    /* Implement for faster copy using arch */
    for (i = 0; i < numbyte; i++) {
        *(cdest + i) = *(csource + i);
        //// printf("Value of the I is %d\n", i);
    }

    return 0;
}



int mem_move_deep(void *dest, const void *source, size_t numbyte) {
    int i;
    // Typecasting the NULL pointer
    char *cdest = (char *)(dest);
    const char *csource = (const char *)(source);
    // If both the pointers are NULL return  error
    if (!(csource && cdest)) {
        return -EINVAL;
    }
    // If  there is no data to copy then return 0
    if (numbyte == 0) {
        return 0;
    }
    // Condition to check if the Source and Destination are getting Overlapped
    // This case covers when the source buffer is getting
    // overlapped from tail
    if ( csource < cdest && cdest < (csource + numbyte) ) {
        while (numbyte != 0) {
            // Start copying from the last location of source
            // to the last location of the destination
            *(cdest + (numbyte - 1)) = *(csource + (numbyte - 1));
            // Point to the nth last location to copy
            //// printf("Value of the numbyte is %zu\n", numbyte - 1);
            numbyte--;
        }
    } else {
        // It covers when the destination buffer is getting overlapped
        // from front and also when both are not getting overlapped
        // in both the cases you will copy data from the
        // starting location of the buffers
        for (i=0; i < numbyte; i++) {
            *(cdest + i) = *(csource + i);
        }
    }
    return 0;
}

int mem_cmp_deep(const void *s1, const void *s2, size_t numbyte) {
    int sum_str1 = 0;
    int sum_str2 = 0;
    int total = 0;

    // Typecasting the NULL pointer
    const char *str1 = (const char *)(s1);
    const char *str2 = (const char *)(s2);

    // If both the pointers are NULL return  error
    if (!(str1 && str1)) {
        return -EINVAL;
    }

    // If there is no data to compare then return 0
    if (numbyte == 0) {
        return 0;
    }
    while ((*str1 != '\0'  && *str2 != '\0') || numbyte == 0) {
        sum_str1 += (int)*str1;
        sum_str2 += (int)*str2;
        str1++;
        str2++;
        numbyte--;
        //// printf("Vaue of str1 is %d", sum_str1);
        //// printf("Vaue of str2 is %d", sum_str2);
    }
    total = sum_str1 - sum_str2;
    return total;
}




int main(void) {
    int flag = 0;
    char example1[50] = "a";
    char example2[50] = "bb";
    unsigned int num = 0;
    num = 0x0C;
    unsigned int *ptr;
    ptr = &num;
    int n = 0;
    int arr[] = {1, 2, 3, 4, 5};
    int *idest = (int *)malloc( sizeof(arr));
    //// idest = &arr[1];
    //// flag = mem_copy_deep( idest, arr1, sizeof(arr1));
    flag = mem_copy_deep( idest, arr, sizeof(arr));
    printf("Value of the flag is %d\n", flag);
    if (flag == 0) {
        printf("copied array is:\n");
        for (int i=0; i < sizeof(arr); i++) {
            printf(" %d\n", *(idest + i));
        }
    }
    flag = mem_cmp_deep(example1, example2, sizeof(example1));
    printf("value of the falg is %d\n", flag);
}


// Have a look at it afterwards
/*
new_calloc(20, 1, 95000);
new_calloc(1000000, 1000000, 255)
new_Calloc(0, 1000000, 255)
new_calloc(20, sizeof(int), 255)
new_calloc(417318237, sizeof(int), 255)

void* new_calloc (unsigned int num_blocks, unsgined int block_size, unsigned int value)
{
    unsigned int memory_size = num_blocks*block_size
    if
    if((memory_size) == 0)
    {
        return NULL
    }
    if(value > 255)
    {
        return NULL
    }
   void *ptr = malloc(memory_size) 
      if(ptr == NULL)
      {
        return NULL
      }
       
       memset(value, ptr);
       
       return(ptr);
}
*/
