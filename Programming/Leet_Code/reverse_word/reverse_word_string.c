#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 *
 * This Function will take the starting ad the ending address of the word and reverse it with
 * the help of the temporary variable
 * 
 * @param word_begin
 *   This is a pointer to the source address of the word from where the data 
 *   needs to be moved
 *   
 * 
 * @param word_destination
 *   This is a pointer to the destination address to where the data needs to be moved  
 * 
 */
void reverse(char *word_begin, char *word_end) {
    //// printf("In reverse function\n");

    // Temporary varibalemto save the copy of the character
    // Thus to prevent the originl copy from overwrite
    char temp;
    while ( word_begin < word_end ) {
        // Store the starting character in the temp variable of type char
        temp = *word_begin;
        // Copy the last character of the word in the begining
        // We have copy of it stored in the temp variable
        *word_begin = *word_end;
        // Update the last location of the word with the starting location
        // which is stored in the temp variable
        *word_end = temp;

        // Increment both the location to point to the next location
        word_end--;
        word_begin++;
    }
}

void reverse_words(char *s ) {
    //// printf("In reverse_word function\n");
    // To keep the track of the whole sentence
    char *temp = s;
    // To keep the track of the starting and the ending of the word
    char *word_begin = s;

    while ( *temp ) {
        temp++;
        if (*temp == '\0') {
            // This if will be executed for the last word in the sentence
            // why temp - 1 because we just want the word and not extra
            // space or null character with it
            reverse(word_begin, temp-1);
        } else if (*temp == ' ') {
            // This condition will execute when there is
            // transition from 1 word to next
            // We are passing temp - 1 because we just want the word and not
            // the null character or space with the
            reverse(word_begin, temp-1);
            // To update the starting of the word which will
            // be temp + 1 at this point
            word_begin = temp + 1;
        }
        //// printf("%s\n", temp);
        //// printf("%s\n", word_begin);
    }
    // Reverse the entire string at the end
    reverse(s, temp-1);
}


int main(void)
{
    char s[] = "I love to play with the string";
    reverse_words(s);
    printf("%s\n", s);
}