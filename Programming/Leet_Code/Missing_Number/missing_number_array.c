#include <stdio.h>
#include <stdlib.h>


int missing_number(unsigned int *ptr, unsigned int max_value) {
    unsigned int result = 0;

    // To store the sum of the numbers upto the max_value
    unsigned int total = 0;

    // To calculate the sum of the array
    unsigned int sum = 0;

    // for loop to calculate the sum of the elements present in the array
    for (int i=0; i < (max_value - 1); i++) {
        sum += *(ptr + i);
    }
    // First N natural numbers upto num
    total = (max_value)*(max_value + 1)/2;

    result = total - sum;
    return(result);
}

int main(void) {
    unsigned int max_value;
    unsigned int result = 0;
    printf("Max value upto which you want to find the missing number:");
    scanf("%d", &max_value);
    unsigned int arr[] = {1, 3, 4};
    result = missing_number(arr, max_value);
    printf("value of the missing number is %d\n", result);
}
