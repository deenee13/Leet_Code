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
        printf("Into if\n");
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
        printf("Value of  j is %d and %d\n", j, i);
       if (!*pattern) {
           j = 0;
           printf("starting of second string is %d\n", (i));
           return begin;
       }
       mainstring = begin + 1;
       i++;
    }
    return NULL;
}

int main(void) {
    char str1[] = "Hello";
    char str2[] = "";
    printf("Str 2 is %s\n", str1);
    const char *ptr = NULL;

    ptr = strstr_deep(str1, str2);
    printf("%s", ptr);
}