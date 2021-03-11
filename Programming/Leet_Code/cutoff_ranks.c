#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>


int countLevelUpPlayers(int cutoffrank, int num, int* scores);

print(int* arr, int num) {

    for (int i = 0; i < num; i++) {
        printf("%d ", *(arr + i));
    }
    printf("\n");
}
int sortarr_highest(int arr_value, int* arr) {

    // Temporary variable for swapping the elements
    int temp;
    // Checking the NULL Condition
    if (arr == NULL) {
        return - EINVAL;
    }
    // For loop from 1st element of the array to the last element
    for (int k=1; k <= (arr_value - 1); k++) {
        // For loop from the 0th element of the array to the value
        // depending upon the k.
        // This is done to prevent the out of bound access of array
        for (int i=0; i <= (arr_value - k -1); i++) {
            if (*(arr+i) < *(arr+i+1)) {
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


void set_rank(int* scores, int num, int* rank) {

    int count = 1;
    int temp = *(scores + 0);

    for (int i = 0; i < num; i++) {
        if (temp == *(scores + i)) {
            *(rank + i) = count;
        } else {
            if(num - 1 == i) {
                *(rank + i) = i+1;
            } else {
                temp = *(scores + i);
                count++;
                *(rank + i) = count;
            }
        }
    }
}

int countLevelUpPlayers(int cutoffrank, int num, int* scores) {

    if (scores == NULL) {
        return -1;
    }
    sortarr_highest(num,scores);
    print(scores, num);
    int* ranks = (int*)malloc(sizeof(int)*num);
    set_rank(scores, num, ranks);
    print(ranks, num);
}




int main(void) {
    /*
    int result;
    int cutoffrank = 4;
    int num = 4;
    int scores[4] = {100, 50, 50, 25};
    result = countLevelUpPlayers(cutoffrank, num, scores);
    */

   char str[20] = "Hello";
   char str1[] = "world";

   strcat(str, str1);
   printf("%s\n", str);

}
