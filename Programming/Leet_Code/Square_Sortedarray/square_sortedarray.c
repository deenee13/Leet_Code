#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Time complexity is 0(n^2) we could use any other algorithm with
// time complexity equal to 0(n.logn)
int bubble_sort(int *arr, const int arr_value) {
    // Temporary variable for swapping the elements
    int temp;
    // Checking the NULL Condition
    if (arr == NULL) {
        return - EINVAL;
    }
    // For loop from 1st element of the array to the last element
    for (int k=1; k <= (arr_value - 1); k++) {
        // For loop from the 0th element of the array to the value
        // depending upon the k
        for (int i=0; i <= (arr_value - k -1); i++) {
            if (*(arr+i) > *(arr+i+1)) {
                //// printf("in bubble_sort %d\n", *(arr+i+1));
                // Swapping algorithm
                temp = *(arr+i);
                *(arr+i) = *(arr+i+1);
                *(arr+i+1) = temp;
            }
        }
    }
    return 0;
}

int* square_sorted(int *arr, int arr_size) {
    for (int i=0; i < arr_size; i++) {
        *(arr + i) = *(arr + i) * *(arr + i);
    }
    bubble_sort(arr, arr_size);
    return(arr);
}

int main(void) {
    int arr[] = {-4, -3, 0, 1, 2};
    int num = sizeof(arr)/sizeof(arr[0]);
    int* arr1 = (int*) malloc(num*sizeof(int));

    int *ptr;
    arr1 = square_sorted(arr, num);
    for (int i=0; i < num; i++) {
        printf("%d\n", *(arr1 + i));
    }
}