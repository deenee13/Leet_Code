#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* add_two_strings(char* str1, char* str2);


char itoa_single(unsigned int value);
unsigned int atoi_single(char value);


char itoa_single(unsigned int value) {
    char result = (value + '0');
    return result;
}

unsigned int atoi_single(char value) {
    int result = (value - '0');
    return result;
}

char* add_two_strings(char* str1, char* str2) {
    unsigned int len1 = strlen(str1);
    unsigned int len2 = strlen(str2);
    char* result = NULL;
    unsigned int sum = 0;
    int i = 0;

    unsigned int length = len1 > len2 ? len1:len2;

    result = (char*)malloc(sizeof(char)*(length));
    char* temp = result;
    result[length] = '\0';
    
    result = result + length - 2;
    str1 = str1 + len1 - 1;
    str2 = str2 + len2 - 1 ;
    

    unsigned int difference = abs((len1 - len2));

    
    while (difference != 0) {
        if (len1 > len2) {
            *result = *str1;
            str1--;
        } else {
            *result = *str2;
            printf("%c and %c\n", *result, *str2);
            str2--; 
        }
        result--;
        difference--;
        i++;
    }
    
    
    /*
    for (int i = 0; i < difference; i++) {
        if (len1 > len2) {
            result[length - i] = str1[length - 1 - i];
        } else {
            result[length - i] = str2[length - 1 - i];
        }
    }*/

    
    while (i != (length)) {
        sum = (atoi_single(*str1) + atoi_single(*str2));
        *result = itoa_single(sum);
        str1--;
        str2--;
        result--;
        i++;
    }
    printf("hi = %c\n", result[0]);
    return (result);
}


int main(void) {
    char str1[] = "1234.5";
    char str2[] = "1234.1234";
    char* result = NULL;
    result = add_two_strings(str1, str2);
    printf("hi = %s\n", result + 1);
}
