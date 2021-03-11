#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NO_OF_CHARS 256

/***
 * This approach is not optimised as time complexity is O(n2) to optimise it we can make us of the
 * hash map. which will store the frequency of each character in an array and then we just need to 
 * iterate through the hashmap to see if any character has frequency 1 or not.
*/
char non_repeated_character(char *str) {
    // To keep the track of the repeated character
    int repeated = 0;
    // To find the length of the input string
    int len = strlen(str);
    // For loop to iterate through the each and every element
    for (int i=0; i < len; i++) {
        // For loop to iterate through the each and every element to check
        // whether any string is getting repeated or not
        for (int j=0; j < len; j++) {
            // If the string is getting repeated break from the
            // loop and see the next element
            if (i != j && *(str+i) == *(str+j)) {
                repeated = 1;
                break;
            }
        }
        // If it is not getting repeated then return that character
        // found the first non repeated character
        if (repeated == 0) {
            return *(str+i);
        }
        // Reinitialise the variable to 0 for the next iteration
        repeated = 0;
    }
    return ' ';
}

char non_repeated_character_optimised(char *str) {
// https://www.geeksforgeeks.org/given-a-string-find-its-first-non-repeating-character/
// check it tomorrow and ask mamu
}

int main(void) {
    char str[] = "geeksforgeeks";
    char result;
    result = non_repeated_character(str);
    printf("Non-Repeated character is  %c\n", result);
}
