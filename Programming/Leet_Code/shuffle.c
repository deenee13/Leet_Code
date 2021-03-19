#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define ENTROPY (10)
#define GET_RANDOM(len) ((rand()) % (len - 1))

enum shuffle_rc {
    SHUFF_SUCCESS = 0,
    SHUFF_ERR,
    SHUFF_MAX,
};


/* Function declaration. */
enum shuffle_rc shuffle(int* arr, size_t len);
static void get_distinct_rands(int* r1, int* r2, int len);


static void get_distinct_rands(int* r1, int* r2, int len) {
    while (*r1 == *r2) {
        *r1 = GET_RANDOM(len);
        *r2 = GET_RANDOM(len);
    }
}

static void swap(int* num1, int* num2) {

    // In this swap method make sure elements are different
    // If they are same then answer will be 0
    /**num1 = *num1 ^ *num2;
    *num2 = *num1 ^ *num2;
    *num1 = *num1 ^ *num2;*/

   *num1 = *num1 + *num2;
   *num2 = *num1 - *num2;
   *num1 = *num1 - *num2;
}

enum shuffle_rc shuffle(int* arr, size_t len) {

    int r1,r2;
    unsigned int total_loop_counts = 0;

    if (arr == NULL || len < 0) {
        return SHUFF_ERR;
    }

    total_loop_counts = (unsigned int)((len * GET_RANDOM(ENTROPY)) + 1);

    for (int i = 0; i < total_loop_counts; i++) {
        r1 = (rand() % (len - 1));
        r2 = (rand() % (len - 1));
        get_distinct_rands(&r1, &r2,len);
        printf("value of r1 = %d and r2 = %d\n", r1, r2);
        printf("arr values before swap are %d and %d\n", arr[r1], arr[r1]);
        swap(&arr[r1], &arr[r2]);
        printf("arr values after swap are %d and %d\n\v", arr[r1], arr[r1]);
    }
    return SHUFF_SUCCESS;
}

int main(void) {
    int arr[5] = {1,2,3,4,5};
    enum shuffle_rc rc = shuffle(arr, 5);
    printf("rc = %d\n", rc);

    for (int i = 0; i < 5; i++) {
        printf("Final Shuffled array is = %d\n", arr[i]);
    }
}