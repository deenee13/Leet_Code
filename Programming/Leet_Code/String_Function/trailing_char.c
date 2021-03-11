#include <stdio.h>
#include <string.h>


void vowel_trail(char* string) {
    unsigned int length = strlen(string);
    int i = 0;
    int j = 1, k = 0;
    while (length != 0) {
        if (*(string + i) == *(string + j)) {
            while(*(string + i) == *(string + j)) {
                j++;
            }
            i = j - 1;
        }
        *(string + k) = *(string + i);
        i++;
        j++;
        k++;
        length--;
    }
}

int main(void) {
   char str[] = "saaaaaammmmmple";
   vowel_trail(str);

   for (int i = 0; i < strlen(str); i++) {
       printf("%c\n", *(str + i));
   }
}