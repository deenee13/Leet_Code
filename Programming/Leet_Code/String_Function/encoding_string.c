#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int encode_string(char* str1);



int encode_string(char* str1) {

    int length = strlen(str1);
    int temp;
    for (int i = 0; i < length; i++) {
        temp = str1[i];

        if (str1[i] + 5 > 'z') {
            temp = (temp + 5) - 26;
        } else {
            temp += 5; 
        }

        str1[i] = temp;
    }
    return 0;
}

int main(void) {

    char str1[] = "rstuv";
    int rc = encode_string(str1);
    printf("result = %s\n", str1);

}