#include "string.h"

/* ways to check if the string reached NULL are:
    1) if (*string == '\0')
    2) if (!*string)      */


unsigned long strlen_deep(const char *s) {
    unsigned long count = 0;
    if (s == NULL) {
        return -EINVAL;
    }
    while (*s) {
        //// printf("Into while loop\n");
        count++;
        s++;
    }
    return count;
}

const char* strstr_deep(const char *mainstring, const char *substring) {
    int i = 0;
    int j = 0;

    if (mainstring == NULL) {
        return NULL;
    }

    if (substring == NULL) {
        return mainstring;
    }

    if (*substring == 0) {
        return 0;
    }

    while (*mainstring) {
        const char *begin = mainstring;
        const char *pattern = substring;
        int temp = i;
        while (*mainstring && *pattern && *pattern == *mainstring) {
            pattern++;
            mainstring++;
            j++;
        }
        printf("Value of j is %d and %d\n", j, i);
       if (!*pattern) {
           j = 0;
           //// printf("starting of second string is %d\n", (i));
           return begin;
        }
        mainstring = begin + 1;
        i++;
    }
    return NULL;
}

void _strcat(char* dst, const char* src) {
    if (!dst) {
        printf("Not enough memory to concatenate string\n");
        exit(EXIT_FAILURE);
    }

    // This will take me to the NULL character of
    // dst string to replace it with the first
    // element of source
    char* ptr = dst + strlen(dst);
    while (*src) {
        *ptr = *src;
        ptr++;
        src++;
    }
    *ptr = '\0';
}

void reverse(char* str, int length) {
    int start = 0;
    int end = length - 1;
    char temp;

    while (start < end) {
        temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}


int _itoa(int value, char* str, int base) {

    /*  To have a check on negative numbers. */
    bool isnegative = false;
    int i = 0;

    /* Handle 0 case explicitly. */
    if (value == 0) {
        str[i] = '0';
        i++;
        str[i] = '\0';
    }

    /* Negative numbers are only handled in base 10. */
    if (base == 10 && value < 0) {
        isnegative = true;
        value = -value;
    }

    /*  Converting the value according to the base. */
    while (value) {
        int rem = value % base;
        str[i] = (rem > 9) ? (rem - 10 + 'A') : (rem + '0');
        i++;
        value = value / base;
    }

    /* If negative value is set put the sign. */
    if (isnegative) {
        str[i] = '-';
        i++;
    }

    /* Null terminating the string. */
    str[i] = '\0';

    /* Reversing the string to get the actual value. */
    reverse(str, i);

    return 1;
}

bool isnumeric(char value) {

    return ((value >= '0') && (value <= '9') ? true : false);
}

int _atoi(const char* str) {

    if (str == NULL) {
        return 0;
    }
    int result = 0;
    int sign = 1;
    int i = 0;

    if (str[i] == '-') {
        sign = -1;
        i++;
    }

    while (str[i] != '\0') {
        if (isnumeric(str[i]) == false) {
            return 0;
        }
        result = result*10 + str[i] - '0';
        i++;
    }
    return sign*result;
}


int main(void) {
    /*
    char str1[] = "Hello";
    char str2[] = "ello";
    //printf("Str 2 is %s\n", str1);
    const char *ptr = NULL;

    ptr = strstr_deep(str1, str2);
    printf("%s\n", ptr);

    char str_1[15] = "Hello";
    char str_2[] = "String";
    char* _ptr = NULL;

    //strcat(str_1, str_2);
    _strcat(str_1, str_2);
    printf("%s\n", str_1);
    */
   
   /*char str1[30];
   _itoa(1234, str1, 2);
   printf("string is %s", str1);*/
    /*
    char str2[5] = "12C";
    int rc = _atoi(str2);
    printf("value is %d", rc);*/


    /* Conversion of Int to char or string */
    char str = '9';     // Declaring string literal as 9
    
   /* 
    * To convert it to int subtract from the '0' literal. 
    * Because to get single digit we do ASCII value of 9 - 
    * ASCII value of 0. which would give us the value 9. 
    * 
    * On other hand if we have int value 9 and want to convert
    * it into string add it with '0' ie 48(ASCII value of 0) + 9
    *  = 57(ASCII value of char '9').
    */
    int data = str - '0'; 
    str = data + '0';
    printf("%c\n", str);
    printf("%d\n", data);
}
